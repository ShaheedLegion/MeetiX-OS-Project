/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHout ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "tasking/thread_manager.hpp"

#include "kernel.hpp"
#include "eva/kernel.h"
#include "logger/logger.hpp"
#include "memory/memory.hpp"
#include "memory/address_space.hpp"
#include "memory/gdt/gdt_macros.hpp"
#include "memory/collections/address_stack.hpp"
#include "memory/physical/pp_allocator.hpp"
#include "memory/physical/pp_reference_tracker.hpp"
#include "memory/temporary_paging_util.hpp"
#include "memory/collections/address_range_pool.hpp"
#include "memory/constants.hpp"
#include "memory/lower_heap.hpp"
#include "system/interrupts/descriptors/ivt.hpp"
#include "utils/string.hpp"
#include "tasking/tasking.hpp"
#include "tasking/process.hpp"
#include "filesystem/filesystem.hpp"
#include "tasking/communication/message_controller.hpp"

/**
 *
 */
PageDirectory ThreadManager::initializePageDirectoryForProcess() 
{

	PageDirectory currentPd = (PageDirectory) CONST_RECURSIVE_PAGE_DIRECTORY_ADDRESS;

	// allocate a page for the directory
	PhysicalAddress physPd = PPallocator::allocate();

	// temporarily map it
	PageDirectory tempPd = (PageDirectory) TemporaryPagingUtil::map(physPd);

	// clone mappings for all tables that are not in user range
	for (uint32_t ti = 0; ti < 1024; ti++) 
	{
		if (!((currentPd[ti] & PAGE_ALIGN_MASK) & PAGE_TABLE_USERSPACE)) tempPd[ti] = currentPd[ti];

		else tempPd[ti] = 0;
	}

	// clone mappings for the lowest 4 MiB
	tempPd[0] = currentPd[0];

	// recursively map to self
	tempPd[1023] = physPd | DEFAULT_KERNEL_TABLE_FLAGS;

	// remove the temporary mapping
	TemporaryPagingUtil::unmap((VirtualAddress) tempPd);

	return (PageDirectory) physPd;
}

/**
 *
 */
PhysicalAddress ThreadManager::forkCurrentPageDirectory(Process *process, Thread *sourceThread, VirtualAddress *outKernelStackVirt, VirtualAddress *outUserStackVirt) 
{
	PageDirectory currentPd = (PageDirectory) CONST_RECURSIVE_PAGE_DIRECTORY_ADDRESS;

	// create the directory
	PhysicalAddress physPd = PPallocator::allocate();

	// temporary map directory
	PageDirectory tempPd = (PageDirectory) TemporaryPagingUtil::map(physPd);

	// deep-copy
	for (uint32_t ti = 1; ti < 1023; ti++) 
	{
		if (currentPd[ti]) 
		{
			// copy kernel tables
			if (ti * 1024 * PAGE_SIZE >= CONST_KERNEL_AREA_START) tempPd[ti] = currentPd[ti]; // TODO make virtual ranges stay

			else if (ti * 1024 * PAGE_SIZE >= CONST_USER_VIRTUAL_RANGES_START) tempPd[ti] = 0; // anything else is deep-copied

			else 
			{
				// get address of the table
				PageTable table = CONST_RECURSIVE_PAGE_TABLE(ti);
				uint32_t tableFlags = currentPd[ti] & (PAGE_ALIGN_MASK);

				// create a new table
				PhysicalAddress clonedTablePhys = PPallocator::allocate();
				PageTable clonedTableTemp = (PageTable) TemporaryPagingUtil::map(clonedTablePhys);

				// copy page table entries
				for (uint32_t pi = 0; pi < 1024; pi++) 
				{
					if (table[pi]) 
					{
						// clone page mappings as read-onle
						clonedTableTemp[pi] = table[pi] & ~PAGE_TABLE_READWRITE;

						// increment reference count on the physical page
						PPreferenceTracker::increment(PAGE_ALIGN_DOWN(table[pi]));
					} 

					else clonedTableTemp[pi] = 0;
				}

				TemporaryPagingUtil::unmap((VirtualAddress) clonedTableTemp);

				// insert into new page directory
				tempPd[ti] = clonedTablePhys | tableFlags;
			}
		} 

		else tempPd[ti] = 0;
	}

	// clone mappings for the lowest 4 MiB
	tempPd[0] = currentPd[0];

	// recursively map to self
	tempPd[1023] = physPd | DEFAULT_KERNEL_TABLE_FLAGS;

	// clone entire user stack area
	VirtualAddress userStackVirtRange = process->virtualRanges.allocate(THREAD_USER_STACK_RESERVED_VIRTUAL_PAGES);
	VirtualAddress userStackStart = userStackVirtRange + (THREAD_USER_STACK_RESERVED_VIRTUAL_PAGES - sourceThread->userStackPages) * PAGE_SIZE;

	for (uint8_t i = 0; i < sourceThread->userStackPages; i++) 
	{
		PhysicalAddress userStackPhys = PPallocator::allocate();
		VirtualAddress userStackPageOff = userStackStart + i * PAGE_SIZE;
		AddressSpace::mapToTemporaryMappedDirectory(tempPd, userStackPageOff, userStackPhys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS, true);

		VirtualAddress userStackPageTemp = TemporaryPagingUtil::map(userStackPhys);
		Memory::copy((uint8_t*) userStackPageTemp, (uint8_t*) (sourceThread->userStackAreaStart + i * PAGE_SIZE), PAGE_SIZE);
		TemporaryPagingUtil::unmap(userStackPageTemp);
	}

	// unmap the temporary mapped directory
	TemporaryPagingUtil::unmap((VirtualAddress) tempPd);

	// copy kernel stack
	VirtualAddress kernelStackVirt = EvaKernel::evaKernelRangePool->allocate(1);
	PhysicalAddress kernelStackPhys = PPallocator::allocate();
	
	AddressSpace::map(kernelStackVirt, kernelStackPhys, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);
	Memory::copy((uint8_t*) kernelStackVirt, (uint8_t*) sourceThread->kernelStackPageVirt, PAGE_SIZE);

	// modify the forked directory
	for (uint32_t ti = 1; ti < 1023; ti++) 
	{
		if (currentPd[ti]) 
		{
			// process area
			if (ti * 1024 * PAGE_SIZE < CONST_USER_VIRTUAL_RANGES_START) 
			{
				// make all pages in all tables read-only
				PageTable table = CONST_RECURSIVE_PAGE_TABLE(ti);
				for (uint32_t pi = 0; pi < 1024; pi++) 
				{
					table[pi] = table[pi] & ~PAGE_TABLE_READWRITE;
				}

				// virtual ranges
			} 

			else if (ti * 1024 * PAGE_SIZE < CONST_KERNEL_AREA_START) 
			{
				// TODO what to do with virtual ranges?
			}
		}
	}

	*outKernelStackVirt = kernelStackVirt;
	*outUserStackVirt = userStackStart;

	return physPd;
}

/**
 *
 */
bool ThreadManager::createThreadUserStack(Process *process, VirtualAddress *outUserStackVirt) 
{
	// prepare user stack virtual range and address
	VirtualAddress userStackVirtRange = process->virtualRanges.allocate(THREAD_USER_STACK_RESERVED_VIRTUAL_PAGES);
	if (userStackVirtRange == 0) 
	{
		if (process->main) 
		{
			logWarn("%! thread creation for process %i failed: no virtual ranges available for stack allocation", "Thread Manager", process->main->id);
		}

		else 
		{
			logWarn("%! main thread creation failed: no virtual ranges available for stack allocation", "Thread Manager");
		}
		
		return false;
	}

	// user stack is at the end of the range
	VirtualAddress userStackVirt = userStackVirtRange + (THREAD_USER_STACK_RESERVED_VIRTUAL_PAGES - 1) * PAGE_SIZE;

	// allocate physical locations
	PhysicalAddress userStackPhys = PPallocator::allocate();

	// map directory temporary and map user stack
	PageDirectory tempPd = (PageDirectory) TemporaryPagingUtil::map((PhysicalAddress) process->pageDirectory);
	AddressSpace::mapToTemporaryMappedDirectory(tempPd, userStackVirt, userStackPhys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
	TemporaryPagingUtil::unmap((VirtualAddress) tempPd);

	// set out parameters
	*outUserStackVirt = userStackVirt;
	return true;
}

/**
 *
 */
bool ThreadManager::createThreadKernelStack(Process *process, VirtualAddress *outKernelStackVirt) 
{
	// perform stack mapping
	VirtualAddress kernelStackVirt = EvaKernel::evaKernelRangePool->allocate(1);
	if (kernelStackVirt == 0) 
	{
		if (process->main) 
		{
			logWarn("%! thread creation for process %i failed: kernel virtual ranges are full", "Thread Manager", process->main->id);
		}

		else 
		{
			logWarn("%! main thread creation failed: kernel virtual ranges are full", "Thread Manager");
		}
		return false;
	}

	// allocate physical locations
	PhysicalAddress kernelStackPhys = PPallocator::allocate();

	// map kernel stack (global space)
	AddressSpace::map(kernelStackVirt, kernelStackPhys, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);

	// set out parameters
	*outKernelStackVirt = kernelStackVirt;
	return true;
}

/**
 *
 */
void ThreadManager::applySecurityLevel(ProcessorState *state, SecurityLevel securityLevel) 
{
	if (securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		state->cs = GDT_DESCRIPTOR_KERNEL_CODE | SEGMENT_SELECTOR_RING0;
		state->ss = GDT_DESCRIPTOR_KERNEL_DATA | SEGMENT_SELECTOR_RING0;
		state->ds = GDT_DESCRIPTOR_KERNEL_DATA | SEGMENT_SELECTOR_RING0;
		state->es = GDT_DESCRIPTOR_KERNEL_DATA | SEGMENT_SELECTOR_RING0;
		state->fs = GDT_DESCRIPTOR_KERNEL_DATA | SEGMENT_SELECTOR_RING0;
		state->gs = GDT_DESCRIPTOR_KERNEL_DATA | SEGMENT_SELECTOR_RING0;
	} 

	else 
	{
		state->cs = GDT_DESCRIPTOR_USER_CODE | SEGMENT_SELECTOR_RING3;
		state->ss = GDT_DESCRIPTOR_USER_DATA | SEGMENT_SELECTOR_RING3;
		state->ds = GDT_DESCRIPTOR_USER_DATA | SEGMENT_SELECTOR_RING3;
		state->es = GDT_DESCRIPTOR_USER_DATA | SEGMENT_SELECTOR_RING3;
		state->fs = GDT_DESCRIPTOR_USER_DATA | SEGMENT_SELECTOR_RING3;
		state->gs = GDT_DESCRIPTOR_USER_DATA | SEGMENT_SELECTOR_RING3;
	}

	if (securityLevel <= SECURITY_LEVEL_DRIVER) state->eflags = state->eflags | 0x3000; // IOPL 3
}

/**
 *
 */
Thread *ThreadManager::fork(Thread *sourceThread) 
{
	Process *parent = sourceThread->process;

	Process *process = new Process(parent->securityLevel);
	process->virtualRanges.initialize(CONST_USER_VIRTUAL_RANGES_START, CONST_KERNEL_AREA_START); // TODO clone virtual ranges during forking

	VirtualAddress kernelStackVirt;
	VirtualAddress userStackVirt;
	process->pageDirectory = (PageDirectory) forkCurrentPageDirectory(process, sourceThread, &kernelStackVirt, &userStackVirt);

	// copy heap information
	process->heapBreak = parent->heapBreak;
	process->heapPages = parent->heapPages;
	process->heapStart = parent->heapStart;
	process->imageEnd = parent->imageEnd;
	process->imageStart = parent->imageStart;

	// create main thread
	VirtualAddress esp0 = kernelStackVirt + PAGE_SIZE;

	Thread *thread = new Thread(THREAD_TYPE_MAIN);
	thread->cpuState = (ProcessorState*) (esp0 - sizeof(ProcessorState));
	thread->kernelStackEsp0 = esp0;

	thread->kernelStackPageVirt = kernelStackVirt;
	thread->userStackAreaStart = userStackVirt;
	thread->userStackPages = sourceThread->userStackPages;

	// link thread to process
	process->parent = parent;
	process->main = thread;
	thread->process = process;

#if LOGGINDEBUG
	dumpTask(thread);
#endif
	return thread;
}

/**
 *
 */
Thread *ThreadManager::createProcess(SecurityLevel securityLevel, Process *parent) 
{
	// create the process
	Process *process = new Process(securityLevel);
	process->pageDirectory = initializePageDirectoryForProcess();
	process->virtualRanges.initialize(CONST_USER_VIRTUAL_RANGES_START, CONST_KERNEL_AREA_START);

	// create main thread
	Thread *mainThread = createThread(process, THREAD_TYPE_MAIN);
	process->parent = parent;
	process->main = mainThread;

#if LOGGINDEBUG
	dumpTask(mainThread);
#endif
	return mainThread;
}

/**
 *
 */
Thread *ThreadManager::createSubThread(Process *process) 
{
	Thread *thread = createThread(process, THREAD_TYPE_SUB);

#if LOGGINDEBUG
	dumpTask(thread);
#endif

	return thread;
}

/**
 *
 */
Thread *ThreadManager::createThread(Process *process, ThreadType type) 
{

	// create the stacks
	VirtualAddress kernelStackPageVirt;
	if (!createThreadKernelStack(process, &kernelStackPageVirt)) return nullptr;

	VirtualAddress userStackAreaStart;
	if (!createThreadUserStack(process, &userStackAreaStart)) return nullptr;

	// calculate stack locations
	VirtualAddress esp0 = kernelStackPageVirt + PAGE_SIZE;
	VirtualAddress esp = userStackAreaStart + PAGE_SIZE;

	// create initial state on the kernel stack
	ProcessorState* state = (ProcessorState*) (esp0 - sizeof(ProcessorState));
	Memory::setBytes(state, 0, sizeof(ProcessorState));
	state->esp = esp;
	state->eip = 0;
	state->eflags = 0x200;

	// apply security level configuration
	applySecurityLevel(state, process->securityLevel);

	// create the thread
	Thread *thread = new Thread(type);
	thread->cpuState = state;
	thread->kernelStackEsp0 = esp0;

	thread->kernelStackPageVirt = kernelStackPageVirt;
	thread->userStackAreaStart = userStackAreaStart;
	thread->userStackPages = 1;

	// link thread to process
	thread->process = process;

	// initialize thread local storage for subthreads
	if (type == THREAD_TYPE_SUB) prepareThreadLocalStorage(thread);

	return thread;
}

/**
 *
 */
Thread *ThreadManager::createProcessVm86(uint8_t interrupt, Vm86Registers &in, Vm86Registers *out) 
{
	Process *process = new Process(SECURITY_LEVEL_KERNEL);
	process->pageDirectory = initializePageDirectoryForProcess();

	// create kernel stack
	VirtualAddress kernelStackVirt;
	if (!createThreadKernelStack(process, &kernelStackVirt)) return nullptr;

	// allocate user stack in lower memory
	VirtualAddress userStackVirt = (uint32_t) LowerHeap::allocate(0x2000);

	// initialize the state
	VirtualAddress esp0 = kernelStackVirt + PAGE_SIZE;

	ProcessorStateVm86* state = (ProcessorStateVm86*) (esp0 - sizeof(ProcessorStateVm86));
	Memory::setBytes(state, 0, sizeof(ProcessorStateVm86));
	state->defaultFrame.eax = in.ax;
	state->defaultFrame.ebx = in.bx;
	state->defaultFrame.ecx = in.cx;
	state->defaultFrame.edx = in.dx;
	state->defaultFrame.ebp = 0;
	state->defaultFrame.esi = in.si;
	state->defaultFrame.edi = in.di;

	state->defaultFrame.eip = FP_OFF(ivt->entry[interrupt]);
	state->defaultFrame.cs = FP_SEG(ivt->entry[interrupt]);
	state->defaultFrame.eflags = 0x20202;
	state->defaultFrame.esp = 0x1000;
	state->defaultFrame.ss = (((userStackVirt & ~(0xFFF)) + 0x1000) >> 4);

	state->gs = 0x00;
	state->fs = 0x00;
	state->es = in.es;
	state->ds = in.ds;

	// create main thread
	Thread *thread = new Thread(THREAD_TYPE_VM86);
	thread->cpuState = (ProcessorState*) state;
	thread->kernelStackEsp0 = esp0;

	thread->kernelStackPageVirt = kernelStackVirt;
	thread->userStackAreaStart = userStackVirt;
	thread->userStackPages = 1;

	thread->getVm86Information()->out = out;

	// assign thread to process
	process->main = thread;
	thread->process = process;

#if LOGGINDEBUG
	dumpTask(thread);
#endif

	return thread;
}

/**
 *
 */
void ThreadManager::prepareThreadLocalStorage(Thread *thread) 
{
	// if tls master copy available, copy it to thread
	Process *process = thread->process;
	if (process->tlsMasterInProcLocation) 
	{
		// calculate size that TLS needs including alignment
		uint32_t tlsMasterAlignedTotalAize = ALIGN_UP(process->tlsMasterTotalsize, process->tlsMasterAlignment);

		// allocate virtual range with aligned size of TLS + size of {UserThread}
		uint32_t requiredSize = tlsMasterAlignedTotalAize + sizeof(UserThread);
		uint32_t requiredPages = PAGE_ALIGN_UP(requiredSize) / PAGE_SIZE;
		VirtualAddress tlsCopyVirt = process->virtualRanges.allocate(requiredPages, PROC_VIRTUAL_RANGE_FLAG_PHYSICAL_OWNER);

		// store executing space
		PageDirectory current = AddressSpace::getCurrentSpace();

		// temporarily switch to target process directory, copy TLS contents
		AddressSpace::switchToSpace(process->pageDirectory);
		for (uint32_t i = 0; i < requiredPages; i++) 
		{
			PhysicalAddress phys = PPallocator::allocate();
			AddressSpace::map(tlsCopyVirt + i * PAGE_SIZE, phys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
			PPreferenceTracker::increment(phys);
		}

		// zero & copy TLS content
		Memory::setBytes((void*) tlsCopyVirt, 0, process->tlsMasterTotalsize);
		Memory::copy((void*) tlsCopyVirt, (void*) process->tlsMasterInProcLocation, process->tlsMasterCopysize);

		// fill user thread
		VirtualAddress userThreadLoc = tlsCopyVirt + tlsMasterAlignedTotalAize;
		UserThread* userThread = (UserThread*) userThreadLoc;
		userThread->self = userThread;

		// switch back
		AddressSpace::switchToSpace(current);

		// set threads TLS location
		thread->userThreadAddr = userThreadLoc;
		thread->tlsCopyVirt = tlsCopyVirt;

		logDebug("%! created tls copy in process %i, thread %i at %h", "Thread Manager", process->main->id, thread->id, thread->tlsCopyVirt);
	}

}

/**
 * Deletes a task and cleans up all its memory. This task must not be in the
 * scheduling list anymore.
 *
 * TODO FIXME XXX
 */
void ThreadManager::deleteTask(Thread *thread) 
{
	DEBUG_INTERFACE_TASK_SET_STATUS(task->id, "dead");

	// clear message queues
	MessageController::clear(thread->id);

	if (thread->type == THREAD_TYPE_SUB) 
	{
		Process *process = thread->process;

		// remove kernel stack
		PPallocator::free(AddressSpace::virtualToPhysical(thread->kernelStackPageVirt));
		//PPallocator::free(AddressSpace::virtualToPhysical(thread->kernelStackEsp0));

		// remove user stack
		PageDirectory currentSpace = AddressSpace::getCurrentSpace();
		AddressSpace::switchToSpace(process->pageDirectory);
		PPallocator::free(AddressSpace::virtualToPhysical(thread->userStackAreaStart));
		AddressSpace::switchToSpace(currentSpace);

		// TODO
	} 

	else if (thread->type == THREAD_TYPE_MAIN) 
	{
		// Here we free everything that the process has created and that is no more
		// needed by anyone.
		delete thread->process;

		//ask to filesystem to close file reference of process
		FileSystem::processClosed(thread->id);

		// deallocate all process intance

	}

	else if (thread->type == THREAD_TYPE_VM86) 
	{

		// User stack is in memory < 1MB so don't unmap
		LowerHeap::free((void*) thread->userStackAreaStart);

		// TODO:
		// figure out vm86 deletion
	}

	logDebug("%! task %i has died, now %i free phys pages", "Thread Manager", thread->id, PPallocator::getFreePageCount());
	
	return;
}

/**
 *
 */
void ThreadManager::dumpTask(Thread *task) 
{
#if LOGGINDEBUG
	logDebug("%! created %s %i", "Thread Manager", (task->type == THREAD_TYPE_SUB ? "thread" : (task->type == THREAD_TYPE_VM86 ? "vm86 process" : "process")), task->id);

	Process *process = task->process;
	logDebugn("%#  process: ");
	if (task->process) 
	{
		logDebug("%i", process->main->id);
	} 

	else 
	{
		logDebug("none");
	}

	logDebug("%#  pagedir:  %h", process->pageDirectory);
	logDebug("%#  security: %h", process->securityLevel);
	logDebug("%#  kernel sp:     %h", task->kernelStackEsp0);
	if (task->type == THREAD_TYPE_VM86) 
	{
		logDebug("%#  cs:ip:  %h:%h", task->cpuState->cs, task->cpuState->eip);
		logDebug("%#  ax: %h bx: %h cx: %h dx: %h", task->cpuState->eax, task->cpuState->ebx, task->cpuState->ecx, task->cpuState->edx);logDebug("%#  user sp:  %h:%h", task->cpuState->esp, task->cpuState->ss);
	} 

	else 
	{
		logDebug("%#  user sp:       %h", task->cpuState->esp);logDebug("%#  entry point:   %h", task->cpuState->eip);
	}
#endif
}

/**
 *
 */
VirtualAddress ThreadManager::getMemoryUsage(Thread *task) 
{

	VirtualAddress total = 0;

	total = total + (task->process->heapPages * PAGE_SIZE);

	return total;
}

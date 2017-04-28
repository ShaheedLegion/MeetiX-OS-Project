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

#include <system/interrupts/handling/interrupt_exception_handler.hpp>
#include <logger/logger.hpp>
#include <tasking/tasking.hpp>
#include <tasking/thread_manager.hpp>
#include <vm86/virtual_8086_monitor.hpp>
#include <system/system.hpp>
#include <memory/address_space.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <memory/physical/pp_reference_tracker.hpp>
#include <memory/temporary_paging_util.hpp>
#include <memory/constants.hpp>

/**
 * Names of the exceptions
 */
static const char *EXCEPTION_NAMES[] = 
{
		"divide error", 					// 0x00
		"debug exception", 					// 0x01
		"non-maskable interrupt exception", // 0x02
		"breakpoint exception", 			// 0x03
		"detected overflow", 				// 0x04
		"out of bounds", 					// 0x05
		"invalid operation code",	 		// 0x06
		"no co-processor", 					// 0x07
		"double fault",						// 0x08
		"co-processor segment overrun", 	// 0x09
		"Bad TSS exception", 				// 0x0A
		"segment not present", 				// 0x0B
		"stack fault", 						// 0x0C
		"general protection fault", 		// 0x0D
		"page fault", 						// 0x0E
		"unknown interrupt", 				// 0x0F
		"co-processor fault", 				// 0x10
		"alignment check exception", 		// 0x11
		"machine check exception", 			// 0x12
		"reserved exception",  				// reserved exceptions
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception",
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception", 
		"reserved exception",
};

/**
 *
 */
uint32_t InterruptExceptionHandler::getCR2() 
{
	uint32_t addr;
	asm volatile("mov %%cr2, %0" : "=r"(addr));
	return addr;
}

/**
 * Dumps the current CPU state to the log file
 */
void InterruptExceptionHandler::dump(Thread *currentThread) 
{
	ProcessorState *cpuState = currentThread->cpuState;
	Process *process = currentThread->process;
	logInfo("%! %s in task %i (process %i)", "exception", EXCEPTION_NAMES[cpuState->intr], currentThread->id, process->main->id);

	if (currentThread->getIdentifier() != 0) 
	{
		logInfo("%# task identified as '%s'", currentThread->getIdentifier());
	}

	if (cpuState->intr == 0x0E) 
	{ // Page fault
		logInfo("%#    accessed address: %h", getCR2());
	}
	logInfo("%#    eip: %h   eflags: %h", cpuState->eip, cpuState->eflags);
	logInfo("%#    eax: %h      ebx: %h", cpuState->eax, cpuState->ebx);
	logInfo("%#    ecx: %h      edx: %h", cpuState->ecx, cpuState->edx);
	logInfo("%#    esp: %h   state@: %h", cpuState->esp, cpuState);
	logInfo("%#   intr: %h    error: %h", cpuState->intr, cpuState->error);
}

/**
 * Handles a general protection fault. If the current task is a VM86 task, the fault is redirected
 * to the Virtual8086 monitor.
 */
Thread *InterruptExceptionHandler::handleGeneralProtectionFault(Thread *currentThread) 
{

	if (currentThread->type == THREAD_TYPE_VM86) 
	{

		VirtualMonitorHandlingResult result = Virtual8086Monitor::handleGpf(currentThread);

		if (result == VIRTUAL_MONITOR_HANDLING_RESULT_SUCCESSFUL) 
		{
			return currentThread;

		} 

		else if (result == VIRTUAL_MONITOR_HANDLING_RESULT_FINISHED) 
		{
			currentThread->alive = false;
			return Tasking::schedule();

		} 

		else if (result == VIRTUAL_MONITOR_HANDLING_RESULT_UNHANDLED_OPCODE) 
		{
			logInfo("%! %i unable to handle gpf for vm86 task", "exception", System::currentProcessorId());
			currentThread->alive = false;
			return Tasking::schedule();
		}
	}

	// Kill process, return with a switch
	Thread *main = currentThread->process->main;
	main->alive = false;
	dump(currentThread);
	logInfo("%! #%i process %i killed due to general protection fault", "exception", System::currentProcessorId(), main->id);
	return Tasking::schedule();
}

/**
 *
 */
void InterruptExceptionHandler::printStackTrace(ProcessorState *state) 
{

	// This function is not very safe, use with caution.
	// Disallowed in anything but debug mode.
#if LOG_LEVEL == LOG_LEVEL_DEBUG
	logInfo("%! Stack trace:", "exception");

	uint32_t * ebp = (uint32_t*) state->ebp;
	for (uint32_t frame = 0; frame < 5; ++frame) 
	{
		uint32_t eip = ebp[1];
		if (eip == 0) 
		{
			break;
		}

		ebp = reinterpret_cast<uint32_t*>(ebp[0]);
		// uint32_t* arguments = &ebp[2];
		logInfo("%#  %h", eip);
	}
#endif
}

/**
 * Handles a page fault
 */
Thread *InterruptExceptionHandler::handlePageFault(Thread *currentThread) 
{

	VirtualAddress accessedVirtual = PAGE_ALIGN_DOWN(getCR2());
	PhysicalAddress accessedPhysical = AddressSpace::virtualToPhysical(accessedVirtual);

	// handle-able stack overflow?
	if (currentThread->type == THREAD_TYPE_SUB || currentThread->type == THREAD_TYPE_MAIN) 
	{

		// calculate the virtual stack area
		VirtualAddress stackAreaStart = currentThread->userStackAreaStart - (THREAD_USER_STACK_RESERVED_VIRTUAL_PAGES - currentThread->userStackPages) * PAGE_SIZE;
		VirtualAddress stackAreaEnd = currentThread->userStackAreaStart + currentThread->userStackPages * PAGE_SIZE;

		if (accessedVirtual >= stackAreaStart && accessedVirtual < stackAreaEnd) 
		{

			// start at the accessed page
			VirtualAddress unmappedNext = PAGE_ALIGN_DOWN(accessedVirtual);
			while (unmappedNext < currentThread->userStackAreaStart) 
			{
				// map physical pages until mapped stack-start is reached
				PhysicalAddress addPagePhys = PPallocator::allocate();
				AddressSpace::map(unmappedNext, addPagePhys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);

				unmappedNext = unmappedNext + PAGE_SIZE;
				currentThread->userStackPages++;
			}

			// set area to new location
			currentThread->userStackAreaStart = PAGE_ALIGN_DOWN(accessedVirtual);

			// continue
			return currentThread;
		}
	}

	// Copy-on-write?
	// Check if within binary image range
	if (accessedVirtual >= currentThread->process->imageStart && accessedVirtual <= currentThread->process->imageEnd) 
	{

		uint32_t ti = TABLE_IN_DIRECTORY_INDEX(accessedVirtual);
		uint32_t pi = PAGE_IN_TABLE_INDEX(accessedVirtual);
		PageTable table = CONST_RECURSIVE_PAGE_TABLE(ti);
		if (table[pi] != 0) 
		{

			// get new physical page
			PhysicalAddress newPhysPhysical = PPallocator::allocate();
			// map it temporary
			VirtualAddress newPhysTemp = TemporaryPagingUtil::map(newPhysPhysical);
			// copy contents
			Memory::copy((uint8_t*) newPhysTemp, (uint8_t*) accessedVirtual, PAGE_SIZE);
			// write new mapping
			AddressSpace::map(accessedVirtual, newPhysPhysical, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS, true);
			// unmap temporary
			TemporaryPagingUtil::unmap(newPhysTemp);

			// new physical page has one more reference, old one has one less
			PPreferenceTracker::increment(newPhysPhysical);

			if (PPreferenceTracker::decrement(accessedPhysical) == 0) 
			{
				PPallocator::free(accessedPhysical);
			}

			logDebug("%! (%i:%i) entry %i/%i copied", "cow", currentThread->process->main->id, currentThread->id, ti, pi);
			return currentThread;
		}
	}

	// raise SIGSEGV in thread
	currentThread->raiseSignal(SIGSEGV);
	logInfo("%! (core %i) raised SIGSEGV in thread %i", "pagefault", System::currentProcessorId(), currentThread->id);
	dump(currentThread);

	return Tasking::schedule();
}

/**
 * Handles a divide error
 */
Thread *InterruptExceptionHandler::handleDivideError(Thread *currentThread) 
{

	dump(currentThread);

	// Let process run, but skip the faulty instruction
	++currentThread->cpuState->eip;
	logInfo("%! #%i thread %i had a divide error", "exception", System::currentProcessorId(), currentThread->id);
	
	return Tasking::schedule();
}

/**
 * Handles a invalid operation code
 */
Thread *InterruptExceptionHandler::handleInvalidOperationCode(Thread *currentThread) 
{

	dump(currentThread);

	// kill thread and process, return with a switch
	Thread *main = currentThread->process->main;
	main->alive = false;
	currentThread->alive = false;
	
	logInfo("%! #%i process %i killed due to invalid operation code %h in thread %i", "exception", System::currentProcessorId(), main->id, *((uint8_t* ) currentThread->cpuState->eip), currentThread->id);
	
	return Tasking::schedule();
}

/**
 *
 */
Thread *InterruptExceptionHandler::handle(Thread *currentThread) 
{

	Logger::manualLock();

	bool resolved = false;

	switch (currentThread->cpuState->intr) 
	{
	case 0x00: 
	{ // Divide error
		currentThread = handleDivideError(currentThread);
		resolved = true;
		break;
	}
	case 0x0E: 
	{ // Page fault
		currentThread = handlePageFault(currentThread);
		resolved = true;
		break;
	}
	case 0x0D: 
	{ // General protection fault
		currentThread = handleGeneralProtectionFault(currentThread);
		resolved = true;
		break;
	}
	case 0x06: 
	{ // Invalid operation code
		currentThread = handleInvalidOperationCode(currentThread);
		resolved = true;
		break;
	}
	}

	if (resolved) 
	{
		Logger::manualUnlock();
		return currentThread;
	}

	// No resolution
	logInfo("%*%! no resolution, hanging system", 0x0C, "exception");
	dump(currentThread);
	for (;;) 
	{
		asm("hlt");
	}
	return 0;
}

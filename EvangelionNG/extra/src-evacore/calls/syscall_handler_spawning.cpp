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

#include <calls/syscall_handler.hpp>
#include <logger/logger.hpp>

#include "eva/kernel.h"
#include "eva/utils/local.hpp"
#include <kernel.hpp>
#include <ramdisk/ramdisk.hpp>
#include <tasking/thread.hpp>
#include <tasking/tasking.hpp>
#include <tasking/thread_manager.hpp>
#include <tasking/wait/waiter_call_vm86.hpp>
#include <memory/address_space.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <memory/physical/pp_reference_tracker.hpp>
#include <executable/elf32_loader.hpp>
#include "debug/debug_interface_kernel.hpp"

#define CREATE_PAGE_IN_SPACE_MAXIMUM_PAGES 100

/**
 *
 */
SYSCALL_HANDLER(createEmptyProcess) 
{
	// Prepare data
	SyscallCreateEmptyProcess *data = (SyscallCreateEmptyProcess*) SYSCALL_DATA(currentThread->cpuState);
	data->processObject = 0;

	Process *newProcess = currentThread->process;

	// Only kernel level
	if (newProcess->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		Thread *emptyMainThread = ThreadManager::createProcess(data->securityLevel, newProcess);
		data->processObject = emptyMainThread;

		logDebug("%! (%i:%i) created empty process %i", "syscall", newProcess->main->id, currentThread->id, emptyMainThread->id);
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: create empty process", "syscall", newProcess->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 * TODO
 */
SYSCALL_HANDLER(cliArgsStore) 
{
	SyscallCliArgsStore *data = (SyscallCliArgsStore*) SYSCALL_DATA(currentThread->cpuState);

	if (currentThread->process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{

		Thread* other = (Thread*) data->processObject;
		Process *otherProcess = other->process;

		otherProcess->cliArguments = new char[CLIARGS_BUFFER_LENGTH];
		uint32_t argsLen = String::length(data->arguments);
		Memory::copy(otherProcess->cliArguments, data->arguments, argsLen);
		otherProcess->cliArguments[argsLen] = 0;

		logDebug("%! task %i stored cli arguments for task %i", "syscall", currentThread->id, other->id);

	} 

	else 
	{
		logWarn("%! task %i tried to store another tasks cli arguments but is not permitted", "syscall", currentThread->id);
	}

	return currentThread;
}

/**
 * TODO
 */
SYSCALL_HANDLER(cliArgsRelease) 
{

	Process *process = currentThread->process;

	SyscallCliArgsRelease *data = (SyscallCliArgsRelease*) SYSCALL_DATA(currentThread->cpuState);

	// Copy args if available
	if (process->cliArguments != 0) 
	{
		Memory::copy(data->buffer, process->cliArguments, CLIARGS_BUFFER_LENGTH);
		data->buffer[String::length(process->cliArguments)] = 0;

		delete process->cliArguments;
		process->cliArguments = 0;
	} 

	else 
	{
		// Null-terminate in case of failure
		data->buffer[0] = 0;
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(createPagesInSpace) 
{

	Process *process = currentThread->process;

	// Prepare data
	SyscallCreatePagesInSpace *data = (SyscallCreatePagesInSpace*) SYSCALL_DATA(currentThread->cpuState);
	data->resultVirtualAddress = 0;

	// Only kernel level
	if (process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{

		Thread* targetThread = (Thread*) data->processObject;
		Process *targetProcess = targetThread->process;

		// Create page in target space
		// Below we do a temporary switch, where 'data' is not available (because we switch to the
		// target userspace, and 'data' is in the current userspace), so we copy the values:
		uint32_t virtualAddressInTargetSpace = data->targetSpaceVirtualAddress;
		uint32_t numberOfPages = data->numberOfPages;

		// Only allow a certain number of pages to be cross-allocated
		if (numberOfPages > 0 && numberOfPages <= CREATE_PAGE_IN_SPACE_MAXIMUM_PAGES) 
		{

			// Adjust the image range of the other process if necessary. This is required so that the kernel
			// can keep track of where the process image lays in this address space.
			if (targetProcess->imageStart == 0 || targetProcess->imageStart > virtualAddressInTargetSpace) 
			{
				targetProcess->imageStart = virtualAddressInTargetSpace;
			}
			
			if (targetProcess->imageEnd == 0 || targetProcess->imageEnd < (virtualAddressInTargetSpace + numberOfPages * PAGE_SIZE)) 
			{
				targetProcess->imageEnd = virtualAddressInTargetSpace + numberOfPages * PAGE_SIZE;
			}

			// Create physical pages and map them into the target space. Remember the physical addresses
			PhysicalAddress physicalPages[numberOfPages];

			// Perform temporary switch to target process and map pages
			AddressSpace::switchToSpace(targetProcess->pageDirectory);
			for (uint32_t i = 0; i < numberOfPages; i++) 
			{
				physicalPages[i] = PPallocator::allocate();
				AddressSpace::map(virtualAddressInTargetSpace + i * PAGE_SIZE, physicalPages[i], DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
				PPreferenceTracker::increment(physicalPages[i]);
			}
			AddressSpace::switchToSpace(process->pageDirectory);

			// Map all pages (which physical addresses are in the array) to the current tasks space
			VirtualAddress virtAddrHere = process->virtualRanges.allocate(numberOfPages, PROC_VIRTUAL_RANGE_FLAG_NONE);
			for (uint32_t i = 0; i < numberOfPages; i++) 
			{
				AddressSpace::map(virtAddrHere + i * PAGE_SIZE, physicalPages[i], DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
			}

			data->resultVirtualAddress = virtAddrHere;

			logDebug("%! (%i:%i) created %i pages in space of process %i. virtual: here %h, there %h", "syscall", process->main->id, currentThread->id, data->numberOfPages, targetThread->id, virtAddrHere, data->targetSpaceVirtualAddress);

		} 

		else 
		{
			logWarn("%! (%i:%i) error: illegal arguments: tried to create pages in a space. %i is not a valid number of pages (1-%i)", "syscall", process->main->id, currentThread->id, data->numberOfPages, CREATE_PAGE_IN_SPACE_MAXIMUM_PAGES);
		}
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: create a page in a space", "syscall", process->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(attachCreatedProcess) 
{
	SyscallAttachCreatedProcess *data = (SyscallAttachCreatedProcess*) SYSCALL_DATA(currentThread->cpuState);

	// Only kernel level
	if (currentThread->process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		Thread* targetMainThread = (Thread*) data->processObject;
		targetMainThread->cpuState->eip = data->eip;
		ThreadManager::prepareThreadLocalStorage(targetMainThread);
		Tasking::addTask(targetMainThread);

		logDebug("%! (%i:%i) attached task %i, starting at eip %h", "syscall", currentThread->process->main->id, currentThread->id, targetMainThread->id, targetMainThread->cpuState->eip);
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: attach created process", "syscall", currentThread->process->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(cancelProcessCreation) 
{
	SyscallCancelProcessCreation *data = (SyscallCancelProcessCreation*) SYSCALL_DATA(currentThread->cpuState);

	// Only kernel level
	if (currentThread->process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		Thread *target = (Thread*) data->processObject;
		ThreadManager::deleteTask(target);

		logDebug("%! (%i:%i) cancelled creation of process %i", "syscall", currentThread->process->main->id, currentThread->id, target->id);
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: cancel process creation", "syscall", currentThread->process->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(getCreatedProcessId) 
{

	Process *process = currentThread->process;

	SyscallGetCreatedProcessID *data = (SyscallGetCreatedProcessID*) SYSCALL_DATA(currentThread->cpuState);

	// Only on kernel level
	if (process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		Thread *createdMainThread = (Thread*) (data->processObject);
		data->resultID = createdMainThread->id;

		logDebug("%! (%i:%i) get id of created process: %i", "syscall", process->main->id, currentThread->id, createdMainThread->id);
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: get created process id", "syscall", process->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(writeTlsMasterForProcess) 
{
	Process *process = currentThread->process;

	SyscallWriteTlsMasterForProcess *data = (SyscallWriteTlsMasterForProcess*) SYSCALL_DATA(currentThread->cpuState);
	data->result = false;

	// Only on kernel level
	if (process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{

		// Get target main thread
		Thread* targetThread = (Thread*) (data->processObject);
		Process *targetProcess = targetThread->process;

		// Get a virtual address range for the TLS master copy
		uint32_t required_pages = PAGE_ALIGN_UP(data->copysize) / PAGE_SIZE;
		VirtualAddress tlsMasterVirt = targetProcess->virtualRanges.allocate(required_pages, PROC_VIRTUAL_RANGE_FLAG_PHYSICAL_OWNER);

		// Temporarily copy master contents to kernel heap because we switch directories
		Local<uint8_t> temporaryStore(new uint8_t[data->copysize]);
		Memory::copy(temporaryStore(), data->content, data->copysize);
		uint32_t temporaryCopySize = data->copysize;

		// Temporarily switch to target process directory, map the pages & zero/copy content
		AddressSpace::switchToSpace(targetProcess->pageDirectory);
		for (uint32_t i = 0; i < required_pages; i++) 
		{
			PhysicalAddress phys = PPallocator::allocate();
			AddressSpace::map(tlsMasterVirt + i * PAGE_SIZE, phys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
			PPreferenceTracker::increment(phys);
		}
		Memory::copy((void*) tlsMasterVirt, temporaryStore(), temporaryCopySize);
		AddressSpace::switchToSpace(process->pageDirectory);

		// Write info to process
		targetProcess->tlsMasterInProcLocation = tlsMasterVirt;
		targetProcess->tlsMasterCopysize = data->copysize;
		targetProcess->tlsMasterTotalsize = data->totalsize;
		targetProcess->tlsMasterAlignment = data->alignment;

		data->result = true;
		logDebug("%! (%i:%i) created tls master for process: %i, at: %h, size: %h, alignment: %h", "syscall", process->main->id, currentThread->id, targetThread->id, targetProcess->tlsMasterInProcLocation, targetProcess->tlsMasterCopysize, targetProcess->tlsMasterAlignment);
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: create tls space in process", "syscall", process->main->id, currentThread->id);
	}

	return currentThread;
}

/**
 *
 */
SYSCALL_HANDLER(ramdiskSpawn) 
{
	Process *process = currentThread->process;

	SyscallRamdiskSpawn *data = (SyscallRamdiskSpawn*) SYSCALL_DATA(currentThread->cpuState);

	if (process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		RamdiskEntry *file = EvaKernel::ramdisk->findAbsolute(data->path);

		if (file) 
		{
			Thread *spawnedThread;
			Process *spawnedProcess;

			Elf32SpawnStatus status = Elf32Loader::spawnFromRamdisk(file, data->securityLevel, &spawnedThread, false, THREAD_PRIORITY_NORMAL);

			if (status == ELF32_SPAWN_STATUS_SUCCESSFUL) 
			{
				data->spawnStatus = RAMDISK_SPAWN_STATUS_SUCCESSFUL;
				spawnedProcess = spawnedThread->process;
				logDebug("%! (%i:%i) spawn '%s' from ramdisk -> %i <%h-%h> #%i", "syscall", process->main->id, currentThread->id, data->path, spawnedProcess->imageStart, spawnedProcess->imageEnd, spawnedThread->id, data->securityLevel);

			} 

			else 
			{
				data->spawnStatus = RAMDISK_SPAWN_STATUS_FAILED_NOT_VALID;
				logWarn("%! task %i tried to spawn ramdisk binary '%s' but validation failed", "syscall", currentThread->id, data->path);
				logWarn("%! (%i:%i) error: invalid elf: spawn '%s' from ramdisk", "syscall", process->main->id, currentThread->id, data->path);
			}

		} 

		else 
		{
			data->spawnStatus = RAMDISK_SPAWN_STATUS_FAILED_NOT_FOUND;
			logWarn("%! (%i:%i) error: file does not exist: spawn '%s' from ramdisk", "syscall", process->main->id, currentThread->id, data->path);
		}
	} 

	else 
	{
		data->spawnStatus = RAMDISK_SPAWN_STATUS_FAILED_NOT_PERMITTED;
		logWarn("%! (%i:%i) error: insufficient permissions: spawn '%s' from ramdisk", "syscall", process->main->id, currentThread->id, data->path);
	}

	return currentThread;
}

/**
 * Returns the entry point for the current thread. If the executing task is not
 * a thread, zero is returned.
 */
SYSCALL_HANDLER(getThreadEntry) 
{

	SyscallGetThreadEntry *data = (SyscallGetThreadEntry*) SYSCALL_DATA(currentThread->cpuState);

	if (currentThread->type == THREAD_TYPE_SUB) 
	{
		data->userEntry = currentThread->threadEntry;
		data->userData = currentThread->userData;
		logDebug("%! process %i retrieved its thread entry %h", "syscall", currentThread->id, currentThread->threadEntry);
	} 

	else 
	{
		data->userEntry = 0;
		logWarn("%! process %i tried to retrieve its thread entry but is a process", "syscall", currentThread->id);
	}

	return currentThread;
}

/**
 * Creates a thread. The given data must contain the initial entry library function
 * and the user entry. The user entry is then added to the thread data and is later
 * retrieved by the library function via getThreadEntry.
 */
SYSCALL_HANDLER(createThread) 
{

	SyscallCreateThread *data = (SyscallCreateThread*) SYSCALL_DATA(currentThread->cpuState);

	logDebug("%! (%i:%i) creates thread", "syscall", currentThread->process->main->id, currentThread->id);

	Thread* thread = ThreadManager::createSubThread(currentThread->process);

	if (thread != 0) 
	{
		logDebug("%! (%i:%i) spawned thread %i", "syscall", currentThread->process->main->id, currentThread->id, thread->id);
		thread->cpuState->eip = (uint32_t) data->initialEntry;
		thread->threadEntry = data->userEntry;
		thread->userData = data->userData;
		Tasking::addTask(thread);

		data->threadID = thread->id;
		data->status = CREATE_THREAD_STATUS_SUCCESSFUL;
	} 

	else 
	{
		logWarn("%! (%i:%i) failed to spawn thread", "syscall", currentThread->process->main->id, currentThread->id);
		data->threadID = -1;
		data->status = CREATE_THREAD_STATUS_FAILED;
	}

	// A process is forced to give away time when creating a thread
	return Tasking::schedule();
}
/**
 *
 */
SYSCALL_HANDLER(configureProcess) 
{

	Process *process = currentThread->process;

	SyscallConfigureProcess *data = (SyscallConfigureProcess*) SYSCALL_DATA(currentThread->cpuState);
	data->result = false;

	// Only on kernel level
	if (process->securityLevel == SECURITY_LEVEL_KERNEL) 
	{
		Thread *targetThread = (Thread*) data->processObject;
		Process *targetProc = (targetThread)->process;

		// store source path in a buffer
		targetProc->sourcePath = new char[PATH_MAX];
		if (targetProc->sourcePath == nullptr) 
		{
			logInfo("%! (%i:%i) error: went out of memory when trying to allocate a source path buffer", "syscall", process->main->id, currentThread->id);

		} 

		else 
		{
			// copy path
			uint32_t sourcePathLen = String::length(data->configuration.sourcePath);
			Memory::copy(targetProc->sourcePath, data->configuration.sourcePath, sourcePathLen);
			targetProc->sourcePath[sourcePathLen] = 0;
			data->result = true;

			DEBUG_INTERFACE_TASK_SET_SOURCE_PATH(targetThread->id, data->configuration.sourcePath);
		}
	} 

	else 
	{
		logWarn("%! (%i:%i) error: insufficient permissions: configure process", "syscall", process->main->id, currentThread->id);
	}

	return currentThread;
}


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

#include "eva/calls/calls.h"
#include <calls/syscall_handler.hpp>

#include <kernel.hpp>
#include <build_config.hpp>
#include <ramdisk/ramdisk.hpp>
#include <executable/elf32_loader.hpp>
#include <logger/logger.hpp>
#include <tasking/tasking.hpp>
#include <tasking/thread_manager.hpp>
#include <tasking/communication/message_controller.hpp>
#include <system/interrupts/handling/interrupt_request_handler.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <memory/address_space.hpp>
#include <memory/temporary_paging_util.hpp>
#include <memory/constants.hpp>
#include <memory/lower_heap.hpp>

// macro for call handler
#define entry(callCode, callHandler) 					\
	case callCode: 										\
		return callHandler(currentThread);				\

/**
 * The system call handler is called by the RequestHandler when a system call is executed.
 * The handler then checks which call was requested by taking it's number from
 * the callers EAX and redirects to the specific system call function.
 */
SYSCALL_HANDLER(handle) 
{
	// get call code from regex
	uint32_t call = SYSCALL_CODE(currentThread->cpuState);

	// search how call is from list
	switch (call) 
	{
		entry(SYSCALL_YIELD, yield);
		entry(SYSCALL_EXIT, exit);
		entry(SYSCALL_EXIT_THREAD, exitThread);
		entry(SYSCALL_GET_PROCESS_ID, getPid);
		entry(SYSCALL_GET_TASK_ID, getTid);
		entry(SYSCALL_GET_PROCESS_ID_FOR_TASK_ID, getPidForTid);
		entry(SYSCALL_GET_PARENT_PROCESS_ID, getParentPid)
		entry(SYSCALL_RAMDISK_SPAWN, ramdiskSpawn);
		entry(SYSCALL_SLEEP, sleep);
		entry(SYSCALL_CREATE_THREAD, createThread);
		entry(SYSCALL_GET_THREAD_ENTRY, getThreadEntry);
		entry(SYSCALL_REGISTER_TASK_IDENTIFIER, taskIdRegister);
		entry(SYSCALL_GET_TASK_FOR_IDENTIFIER, taskIdGet);

		entry(SYSCALL_MESSAGE_SEND, sendMessage);
		entry(SYSCALL_MESSAGE_RECEIVE, receiveMessage);

		entry(SYSCALL_WAIT_FOR_IRQ, waitForIrq);
		entry(SYSCALL_ALLOCATE_MEMORY, allocMem);
		entry(SYSCALL_SHARE_MEMORY, shareMem);
		entry(SYSCALL_MAP_MMIO_AREA, mapMmio);
		entry(SYSCALL_UNMAP, unmap);
		entry(SYSCALL_SBRK, sbrk);
		entry(SYSCALL_CALL_VM86, callVm86);
		entry(SYSCALL_LOWER_MEMORY_ALLOCATE, lowerMalloc);
		entry(SYSCALL_LOWER_MEMORY_FREE, lowerFree);
		entry(SYSCALL_LOG, log);
		entry(SYSCALL_RAMDISK_FIND, ramdiskFind);
		entry(SYSCALL_RAMDISK_FIND_CHILD, ramdiskFindChild);
		entry(SYSCALL_RAMDISK_INFO, ramdiskInfo);
		entry(SYSCALL_RAMDISK_READ, ramdiskRead);
		entry(SYSCALL_RAMDISK_CHILD_COUNT, ramdiskChildCount);
		entry(SYSCALL_RAMDISK_CHILD_AT, ramdiskChildAt);
		entry(SYSCALL_SET_VIDEO_LOG, setVideoLog);
		entry(SYSCALL_KILL, kill);
		entry(SYSCALL_REGISTER_IRQ_HANDLER, registerIrqHandler);
		entry(SYSCALL_RESTORE_INTERRUPTED_STATE, restoreInterruptedState);
		entry(SYSCALL_REGISTER_SIGNAL_HANDLER, registerSignalHandler);
		entry(SYSCALL_RAISE_SIGNAL, raiseSignal);
		entry(SYSCALL_CONFIGURE_PROCESS, configureProcess);

		entry(SYSCALL_CREATE_EMPTY_PROCESS, createEmptyProcess);
		entry(SYSCALL_CREATE_PAGES_IN_SPACE, createPagesInSpace);
		entry(SYSCALL_ATTACH_CREATED_PROCESS, attachCreatedProcess);
		entry(SYSCALL_CANCEL_PROCESS_CREATION, cancelProcessCreation);
		entry(SYSCALL_GET_CREATED_PROCESS_ID, getCreatedProcessId);
		entry(SYSCALL_WRITE_TLS_MASTER_FOR_PROCESS, writeTlsMasterForProcess);
		entry(SYSCALL_STORE_CLI_ARGUMENTS, cliArgsStore);
		entry(SYSCALL_RELEASE_CLI_ARGUMENTS, cliArgsRelease);

		entry(SYSCALL_TEST, test);
		entry(SYSCALL_EVAQUERY, evaQuery);
		entry(SYSCALL_ATOMIC_LOCK, atomicWait);
		entry(SYSCALL_GET_MILLISECONDS, millis);
		entry(SYSCALL_FORK, fork);
		entry(SYSCALL_JOIN, join);
		entry(SYSCALL_GET_WORKING_DIRECTORY, getWorkingDirectory);
		entry(SYSCALL_GET_EXECUTABLE_PATH, getExecutablePath);
		entry(SYSCALL_SET_WORKING_DIRECTORY, setWorkingDirectory);
		entry(SYSCALL_FS_OPEN, fsOpen);
		entry(SYSCALL_FS_READ, fsRead);
		entry(SYSCALL_FS_WRITE, fsWrite);
		entry(SYSCALL_FS_CLOSE, fsClose);
		entry(SYSCALL_FS_STAT, fsStat);
		entry(SYSCALL_FS_FSTAT, fsFstat);
		entry(SYSCALL_FS_CLONEFD, fsClonefd);
		entry(SYSCALL_FS_PIPE, fsPipe);
		entry(SYSCALL_FS_LENGTH, fsLength);
		entry(SYSCALL_FS_SEEK, fsSeek);
		entry(SYSCALL_FS_TELL, fsTell);
		entry(SYSCALL_FS_OPEN_DIRECTORY, fsOpenDirectory);
		entry(SYSCALL_FS_READ_DIRECTORY, fsReadDirectory);

		entry(SYSCALL_FS_REGISTER_AS_DELEGATE, fsRegisterAsDelegate);
		entry(SYSCALL_FS_SET_TRANSACTION_STATUS, fsSetTransactionStatus);
		entry(SYSCALL_FS_CREATE_NODE, fsCreateNode);

		entry(SYSCALL_UNAME, uname);
		entry(SYSCALL_SYSINFO, sysinfo);

		entry(SYSCALL_REBOOT, reboot);
		entry(SYSCALL_SHUTDOWN, shutdown);

		entry(SYSCALL_SOUND, sound);
		entry(SYSCALL_SOUNDMUTE, soundMute);		
	}

	// The system call could not be handled, this might mean that the
	// process was compiled for a deprecated/messed up API library and
	// is therefore not able to run well.
	logDebug("%! process %i tried to use non-existing syscall %i", "syscall", currentThread->id, SYSCALL_CODE(currentThread->cpuState));
	
	// kill task for security
	currentThread->alive = false;
	
	// schedule to next process
	return Tasking::schedule();
}

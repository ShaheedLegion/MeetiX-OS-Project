/* 
*MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
 *
 *This program is free software; you can redistribute it and/or
 *modify it under the terms of the GNU General Public License
 *as published by the Free Software Foundation; either version 2
 *of the License, or (char  *rgumentat your option) any later version.
 * *This program is distributed in the hope that it will be useful,
 *but WITHout ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 * *You should have received a copy of the GNU General Public License
 *along with this program; if not, write to the Free Software
 *Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef EVA_SYSTEM_CALLS
#define EVA_SYSTEM_CALLS

#include <utils/string.hpp>
#include <memory/paging.hpp>
#include <memory/memory.hpp>
#include <system/processor_state.hpp>
#include <tasking/thread.hpp>

// Macros for call code and data
#define SYSCALL_CODE(state)				state->eax
#define SYSCALL_DATA(state)				state->ebx

// Call handler macro
#define SYSCALL_HANDLER(callName)		Thread *SysCallHandler::callName(Thread *currentThread)

/**  
 *  
 */
class SysCallHandler 
{
public:
	// Main Handler of syscall
	static Thread *handle(Thread *state);

private:
	// Process interaction
	static Thread *yield(Thread *state);
	static Thread *exit(Thread *state);
	static Thread *exitThread(Thread *state);
	static Thread *sleep(Thread *state);
	static Thread *getPid(Thread *state);
	static Thread *getTid(Thread *state);
	static Thread *getPidForTid(Thread *state);
	static Thread *getParentPid(Thread *state);
	static Thread *ramdiskSpawn(Thread *state);
	static Thread *createThread(Thread *state);
	static Thread *getThreadEntry(Thread *state);
	static Thread *waitForIrq(Thread *state);
	static Thread *callVm86(Thread *state);
	static Thread *cliArgsStore(Thread *state);
	static Thread *cliArgsRelease(Thread *state);
	static Thread *millis(Thread *state);
	static Thread *kill(Thread *state);
	static Thread *registerIrqHandler(Thread *state);
	static Thread *restoreInterruptedState(Thread *state);
	static Thread *registerSignalHandler(Thread *state);
	static Thread *raiseSignal(Thread *state);

	// Process information and waiting
	static Thread *taskIdRegister(Thread *state);
	static Thread *taskIdGet(Thread *state);
	static Thread *fork(Thread *state);
	static Thread *join(Thread *state);
	static Thread *atomicWait(Thread *state);

	// Process creation
	static Thread *createEmptyProcess(Thread *state);
	static Thread *createPagesInSpace(Thread *state);
	static Thread *attachCreatedProcess(Thread *state);
	static Thread *cancelProcessCreation(Thread *state);
	static Thread *getCreatedProcessId(Thread *state);
	static Thread *writeTlsMasterForProcess(Thread *state);
	static Thread *configureProcess(Thread *state);

	// Process messaging
	static Thread *sendMessage(Thread *state);
	static Thread *receiveMessage(Thread *state);

	// Memory management
	static Thread *allocMem(Thread *state);
	static Thread *shareMem(Thread *state);
	static Thread *mapMmio(Thread *state);
	static Thread *sbrk(Thread *state);
	static Thread *lowerMalloc(Thread *state);
	static Thread *lowerFree(Thread *state);
	static Thread *unmap(Thread *state);

	// Log 
	static Thread *log(Thread *state);
	static Thread *setVideoLog(Thread *state);
	static Thread *test(Thread *state);
	static Thread *evaQuery(Thread *state);

	// RamdDisk utils
	static Thread *ramdiskFind(Thread *state);
	static Thread *ramdiskFindChild(Thread *state);
	static Thread *ramdiskInfo(Thread *state);
	static Thread *ramdiskRead(Thread *state);
	static Thread *ramdiskChildCount(Thread *state);
	static Thread *ramdiskChildAt(Thread *state);

	// File operation
	static Thread *fsOpen(Thread *state);
	static Thread *fsRead(Thread *state);
	static Thread *fsWrite(Thread *state);
	static Thread *fsClose(Thread *state);
	static Thread *fsSeek(Thread *state);
	static Thread *fsLength(Thread *state);

	// Fs operation
	static Thread *fsStat(Thread *state);
	static Thread *fsFstat(Thread *state);
	static Thread *fsClonefd(Thread *state);
	static Thread *fsPipe(Thread *state);
	static Thread *fsTell(Thread *state);
	static Thread *fsOpenDirectory(Thread *state);
	static Thread *fsReadDirectory(Thread *state);

	// Path utils
	static Thread *getExecutablePath(Thread *state);
	static Thread *getWorkingDirectory(Thread *state);
	static Thread *setWorkingDirectory(Thread *state);

	// Fs mount
	static Thread *fsRegisterAsDelegate(Thread *state);
	static Thread *fsSetTransactionStatus(Thread *state);
	static Thread *fsCreateNode(Thread *state);

	// info
	static Thread *uname(Thread *state);
	static Thread *sysinfo(Thread *state);

	// Healt utils
	static Thread *reboot(Thread *state);
	static Thread *shutdown(Thread *state);

	// Sound 
	static Thread *sound(Thread *state);
	static Thread *soundMute(Thread *state);
};

#endif

/*
*MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (char *argumentat your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHout ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef EVA_KERNEL_TASKINThreadMANAGER
#define EVA_KERNEL_TASKINThreadMANAGER

#include <system/processor_state.hpp>

#include "eva/kernel.h"
#include "eva/stdint.h"
#include <tasking/thread.hpp>

/**
 *
 */
class ThreadManager 
{
public:
	static Thread *fork(Thread *task);

	static Thread *createProcess(SecurityLevel securityLevel, Process *parent);
	static Thread *createSubThread(Process *process);

	static Thread *createProcessVm86(uint8_t interrupt, Vm86Registers &in, Vm86Registers *out);

	static void prepareThreadLocalStorage(Thread *thread);

	static void deleteTask(Thread *task);

	static bool registerTaskForIdentifier(Thread *task, const char *identifier);
	static Thread *getTaskForIdentifier(const char *identifier);

	static VirtualAddress getMemoryUsage(Thread *task);

private:
	static void applySecurityLevel(ProcessorState *state, SecurityLevel securityLevel);

	static Thread *createThread(Process *process, ThreadType type);

	/**
	 *Allocates and initializes a new page directory used for process creation.
	 */
	static PageDirectory initializePageDirectoryForProcess();

	static PhysicalAddress forkCurrentPageDirectory(Process *process, Thread *current, VirtualAddress *outKernelStackVirt, VirtualAddress *outUserStackVirt);

	/**
	 *Allocates the user thread stack for a new thread in the given process.
	 */
	static bool createThreadUserStack(Process *process, VirtualAddress *outUserStackVirt);

	/**
	 *Allocates the kernel thread stack for a new thread in the given process.
	 */
	static bool createThreadKernelStack(Process *process, VirtualAddress *outKernelStackVirt);

	static void dumpTask(Thread *task);
};

#endif

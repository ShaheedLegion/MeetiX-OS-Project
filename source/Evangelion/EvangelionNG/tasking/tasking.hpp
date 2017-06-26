/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#ifndef EVA_TASKING_TASKING
#define EVA_TASKING_TASKING

#include <system/ProcessorState.hpp>

#include "eva/kernel.h"
#include "eva/stdint.h"
#include <tasking/thread.hpp>
#include <tasking/process.hpp>
#include <tasking/scheduling/scheduler.hpp>

/**
 *
 */
class Tasking
{
public:

	/**
	 * Called once by the BSP
	 */
	static void initialize();

	/**
	 * Called by each core (including BSP)
	 */
	static void enableForThisCore();

	/**
	 * Saves the current CPU state and returns the current thread.
	 */
	static Thread *save(ProcessorState *cpuState);

	/**
	 * Called to switch tasks. Function returns the task to execute next.
	 */
	static Thread *schedule();

	/**
	 * Adds the task to the least loaded cores scheduler
	 */
	static void addTask(Thread *proc, bool enforceCurrentCore = false);

	/**
	 * Pushes the given thread to the top of the wait queue.
	 */
	static void increaseWaitPriority(Thread *proc);

	/**
	 * Returns the current task on the current core
	 */
	static Thread *lastThread();

	/**
	 * Kills all threads of the given process within all schedulers.
	 * Returns true if all are dead.
	 *
	 * @param process
	 * 		the process of which the threads shall be killed
	 */
	static void removeThreads(Process *process);

	/**
	 * Returns the current scheduler on the current core
	 */
	static Scheduler *currentScheduler();

	/**
	 *
	 */
	static Thread *getTaskById(uint32_t id);

	/**
	 *
	 */
	static Thread *getTaskByIdentifier(const char *identifier);

	/**
	 *
	 */
	static bool registerTaskForIdentifier(Thread *task, const char *newIdentifier);

	/**
	 *
	 */
	static Thread *fork(Thread *currentThread);

	/**
	 * Counts the number of tasks in all schedulers.
	 */
	static uint32_t count();

	/**
	 *
	 */
	static uint32_t getTaskIds(Tid *out, uint32_t len);

};

#endif

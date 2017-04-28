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

#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_

#include <system/processor_state.hpp>

#include "eva/stdint.h"
#include <utils/list_entry.hpp>
#include <tasking/thread.hpp>
#include <system/smp/global_recursive_lock.hpp>

typedef ListEntry<Thread*> TaskEntry;

/**
 * The scheduler is responsible for determining which task is the next one to
 * be scheduled and to apply the actual switching.
 */
class Scheduler 
{
private:
	uint64_t milliseconds;
	uint32_t coreId;

	TaskEntry *waitQueue;
	TaskEntry *runQueue;
	TaskEntry *idleEntry;
	TaskEntry *currentEntry;

	/**
	 * Chooses which thread to execute next and sets it as the current entry.
	 */
	void _selectNextTask();

	/**
	 * Performs the actual context switch.
	 *
	 * @param thread
	 * 		the thread to switch to
	 */
	void _applyContextSwitch(Thread *thread);

	/**
	 * Removes the given thread from its queue.
	 *
	 * @param thread
	 * 		the thread to remove
	 */
	void _remove(Thread *thread);

	/**
	 * Checks whether the given thread has died. Removes it (and all child threads).
	 *
	 * @param thread
	 * 		the thread to check
	 * @return whether the thread as died
	 */
	bool _checkAliveState(Thread *thread);

	/**
	 * Finishs the context switch by setting userspace specific values.
	 *
	 * @param thread
	 * 		thread to finish the switch for
	 */
	void _finishSwitch(Thread *thread);

	/**
	 * Removes a thread from a queue.
	 *
	 * @param queue_head
	 * 		pointer to a queues head
	 * @param thread
	 * 		the thread to remove
	 * @return the thread entry or nullptr if not found
	 */
	TaskEntry* _removeFromQueue(TaskEntry **queueHead, Thread *thread);

	/**
	 * Prints a deadlock warning for the current thread.
	 */
	void _printDeadlockWarning();

	/**
	 * Processes all threads in the wait queue. Checks the waiting state for each
	 * thread in the wait queue.
	 */
	void _processWaitQueue();

	/**
	 * Checks the thread for any waiting operations, performs these and moves the
	 * thread to the run queue if possible.
	 *
	 * @param thread
	 * 		the thread to check
	 */
	void _checkWaitingState(Thread *thread);

public:
	/**
	 * Creates a new scheduler for the given core.
	 *
	 * @param coreId
	 * 		id of the core
	 */
	Scheduler(uint32_t coreId);

	/**
	 * Saves the processor state to the thread structure that was
	 * last executed and returns a pointer to this thread. If there is
	 * no last thread, scheduling is performed.
	 *
	 * @param cpuState
	 * 		pointer to the CPU structure on the process stack
	 * @return the last executed thread
	 */
	Thread *save(ProcessorState *cpuState);

	/**
	 * Performs the scheduling. This processes the waiting tasks and then
	 * does the scheduling algorithm to pick the next task.
	 *
	 * @return the next task to execute
	 */
	Thread *schedule();

	/**
	 * Adds the given thread to the run queue.
	 *
	 * @param thread
	 * 		the thread to schedule
	 */
	void add(Thread *newThread);

	/**
	 * Moves the given thread to the top of the wait queue so it is
	 * processed as soon as possible.
	 *
	 * @param thread
	 * 		the thread to push
	 */
	void increaseWaitPriority(Thread *thread);

	/**
	 * Generates a value that is used to representate the load for this
	 * scheduler.
	 *
	 * @return a numeric load value
	 */
	uint32_t calculateLoad();

	/**
	 * @return the thread that was executed last
	 */
	Thread *lastThread();

	/**
	 * Retrieves the task with the given id.
	 *
	 * @return the thread or nullptr if not existant
	 */
	Thread *getTaskById(Tid id);

	/**
	 * Retrieves the task with the given identifier.
	 *
	 * @param identifier
	 * 		the task identifier to find
	 * @return the thread or nullptr if not existant
	 */
	Thread *getTaskByIdentifier(const char* identifier);

	/**
	 * Removes the given thread from the wait queue and adds it to the
	 * run queue.
	 *
	 * @param thread
	 * 		the thread to move
	 */
	void moveToRunQueue(Thread *thread);

	/**
	 * Removes the given thread from the run queue and adds it to the
	 * wait queue.
	 *
	 * @param thread
	 * 		the thread to move
	 */
	void moveToWaitQueue(Thread *thread);

	/**
	 * Sets all threads of the given process to not alive and returns
	 * whether there where any threads that are still alive.
	 *
	 * @param process
	 * 		the process of which all threads shall be killed
	 */
	void removeThreads(Process *process);

	/**
	 * Is called by the timer interrupt to count time since the scheduler is
	 * running. Increases the millisecond count by the amount of time that each
	 * timer tick takes.
	 */
	void updateMilliseconds();

	/**
	 * @return the number of milliseconds that this scheduler is running.
	 */
	uint64_t getMilliseconds();

	/**
	 * Counts the number of tasks in this scheduler.
	 */
	uint32_t count();

	/**
	 * The position is solely a logical thing used for listing tasks in the system.
	 */
	Thread *getAtPosition(uint32_t position);

};

#endif

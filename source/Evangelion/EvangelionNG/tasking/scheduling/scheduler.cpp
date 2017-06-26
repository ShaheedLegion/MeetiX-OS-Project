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

#include <tasking/scheduling/scheduler.hpp>
#include <tasking/wait/WaiterSleep.hpp>
#include <system/interrupts/lapic.hpp>
#include <logger/logger.hpp>
#include <tasking/ThreadManager.hpp>
#include <memory/AddressSpace.hpp>
#include <memory/gdt/GdtManager.hpp>
#include <utils/string.hpp>
#include <EvangelionNG.hpp>
#include "debug/DebugInterfaceKernel.hpp"

/**
 *
 */
Scheduler::Scheduler(uint32_t coreId) : milliseconds(0), coreId(coreId), waitQueue(0), runQueue(0), idleEntry(0), currentEntry(0) {}

/**
 *
 */
void Scheduler::add(Thread *newThread)
{
	// set the scheduler on the task
	newThread->scheduler = this;

	// the idle task is not added to a queue
	if (newThread->priority == THREAD_PRIORITY_IDLE)
	{
		TaskEntry *entry = new TaskEntry();
		entry->value = newThread;
		entry->next = 0;
		idleEntry = entry;
	}

	else
	{
		// add task to run queue
		TaskEntry *entry = new TaskEntry();
		entry->value = newThread;
		entry->next = runQueue;
		runQueue = entry;
	}

	logDebug("%! task %i assigned to core %i", "scheduler", newThread->id, coreId);
}

/**
 *
 */
void Scheduler::removeThreads(Process *process)
{
	// set all threads in run queue to dead
	TaskEntry *entry = runQueue;
	while (entry)
	{
		TaskEntry *next = entry->next;
		if (entry->value->process->main->id == process->main->id) _remove(entry->value);

		entry = next;
	}

	// set all threads in wait queue to dead
	entry = waitQueue;
	while (entry)
	{
		TaskEntry *next = entry->next;
		if (entry->value->process->main->id == process->main->id) _remove(entry->value);
		entry = next;
	}
}

/**
 *
 */
Thread *Scheduler::lastThread()
{
	if (currentEntry) return currentEntry->value;
	return nullptr;
}

/**
 *
 */
uint32_t Scheduler::calculateLoad()
{
	uint32_t load = 0;

	// TODO improve load calculation
	TaskEntry *entry = runQueue;
	while (entry)
	{
		++load;
		entry = entry->next;
	}
	entry = waitQueue;
	while (entry)
	{
		++load;
		entry = entry->next;
	}

	return load;
}

/**
 *
 */
void Scheduler::updateMilliseconds()
{
	milliseconds = milliseconds + APIC_MILLISECONDS_PER_TICK;

#if DEBUG_THREAD_DUMPING
	// debug dump
	static uint64_t lastDebugoutMillis = 0;
	if (milliseconds - lastDebugoutMillis > 10000)
	{
		lastDebugoutMillis = milliseconds;

		logInfo("----------------");
		TaskEntry *entry = runQueue;
		while (entry)
		{
			Thread *thr = entry->value;
			logInfo("%# running - %i:%i, eip: %h, waiter: %s, name: %s, rounds: %i", thr->process->main->id, thr->id, thr->cpuState->eip,
					(thr->waitManager == nullptr ? "-" : thr->waitManager->debugName()), (thr->getIdentifier() == 0 ? "-" : thr->getIdentifier()),
					thr->rounds);
			entry = entry->next;
		}
		entry = waitQueue;
		while (entry)
		{
			Thread *thr = entry->value;
			logInfo("%# waiting - %i:%i, eip: %h, waiter: %s, name: %s, rounds: %i", thr->process->main->id, thr->id, thr->cpuState->eip,
					(thr->waitManager == nullptr ? "-" : thr->waitManager->debugName()), (thr->getIdentifier() == 0 ? "-" : thr->getIdentifier()),
					thr->rounds);
			entry = entry->next;
		}
	}
#endif

#if DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_FULL
	static uint64_t lastProcessorTimeUpdate = 0;
	if (milliseconds - lastProcessorTimeUpdate > 500)
	{
		lastProcessorTimeUpdate = milliseconds;

		TaskEntry *entry = runQueue;
		while (entry)
		{
			Thread *thr = entry->value;
			DEBUG_INTERFACE_TASK_SET_ROUNDS(thr->id, thr->rounds);
			thr->rounds = 0;
			entry = entry->next;
		}
		entry = waitQueue;
		while (entry)
		{
			Thread *thr = entry->value;
			DEBUG_INTERFACE_TASK_SET_ROUNDS(thr->id, thr->rounds);
			thr->rounds = 0;
			entry = entry->next;
		}
	}
#endif
}

/**
 *
 */
uint64_t Scheduler::getMilliseconds()
{
	return milliseconds;
}

/**
 *
 */
Thread *Scheduler::save(ProcessorState *cpuState)
{
	// store processor state in current task
	if (currentEntry)
	{
		currentEntry->value->cpuState = cpuState;
		return currentEntry->value;
	}

	// no last thread? do scheduling
	return schedule();
}

/**
 *
 */
Thread *Scheduler::schedule()
{
	// store which entry is running
	TaskEntry *runningEntry = currentEntry;

	// process wait queue (overwrites current entry)
	_processWaitQueue();

	// continue scheduling there
	currentEntry = runningEntry;

	// select next task to run
	while (true)
	{
		// when scheduling for the first time, there's not current task
		if (!currentEntry) currentEntry = runQueue;

		else
		{
			// select next in run queue
			currentEntry = currentEntry->next;
			if (!currentEntry) currentEntry = runQueue;
		}

		// no task in run queue? select idle thread
		if (!currentEntry)
		{
			currentEntry = idleEntry;
			if (!currentEntry) EvaKernel::panic("%! idle thread does not exist on core %i", "scheduler", coreId);
		}

		// sanity check
		if (currentEntry->value->waitManager) EvaKernel::panic("task %i is in run queue had wait manager '%s'", currentEntry->value->id, currentEntry->value->waitManager->debugName());

		// try to switch
		_applyContextSwitch(currentEntry->value);

		// remove it task is no more alive
		if (!_checkAliveState(currentEntry->value))
		{
			currentEntry = 0;
			continue;
		}

		// task was successfully selected & switched to
		break;
	}

	// finish the switch
	_finishSwitch(currentEntry->value);
	++currentEntry->value->rounds;

	return currentEntry->value;
}

/**
 *
 */
void Scheduler::_applyContextSwitch(Thread *thread)
{
	// switch to the address space
	AddressSpace::switchToSpace(thread->process->pageDirectory);

	// set ESP0 in the TSS
	GdtManager::setTssEsp0(thread->kernelStackEsp0);
}

/**
 *
 */
bool Scheduler::_checkAliveState(Thread *thread)
{
	// Eliminate if dead
	if (!thread->alive)
	{
		if (thread->type == THREAD_TYPE_MAIN) Tasking::removeThreads(thread->process);
		else _remove(thread);

		return false;
	}

	return true;
}

/**
 *
 */
void Scheduler::_finishSwitch(Thread *thread)
{
	// write user thread address to GDT
	GdtManager::setUserThreadAddress(thread->userThreadAddr);

	// set GS of thread to user pointer segment
	thread->cpuState->gs = 0x30;
}

/**
 *
 */
void Scheduler::_remove(Thread *thread)
{
	// remove from run queue
	TaskEntry *entry = _removeFromQueue(&runQueue, thread);

	// if it was not in run queue, get it from wait queue
	if (entry == 0) entry = _removeFromQueue(&waitQueue, thread);

	// if it was in neither queues, thats weird
	if (entry == 0)
	{
		logWarn("%! failed to properly delete thread %i, was not assigned to a queue", "scheduler", thread->id);
		return;
	}

	// delete entry and task
	delete entry;
	ThreadManager::deleteTask(thread);
}

/**
 *
 */
void Scheduler::_printDeadlockWarning()
{
	char *taskName = (char*) "?";
	if (currentEntry->value->getIdentifier() != 0) taskName = (char*) currentEntry->value->getIdentifier();

	logDebug("%! thread %i (process %i, named '%s') waits for '%s'", "deadlock-detector", currentEntry->value->id, currentEntry->value->process->main->id, taskName, currentEntry->value->waitManager->debugName());
}

/**
 *
 */
Thread *Scheduler::getTaskById(Tid id)
{
	Thread *thread = 0;

	TaskEntry *entry = runQueue;
	while (entry)
	{
		if (entry->value->alive && entry->value->id == id)
		{
			thread = entry->value;
			break;
		}
		entry = entry->next;
	}

	if (!thread)
	{
		entry = waitQueue;
		while (entry)
		{
			if (entry->value->alive && entry->value->id == id)
			{
				thread = entry->value;
				break;
			}
			entry = entry->next;
		}
	}

	return thread;
}

/**
 *
 */
Thread *Scheduler::getTaskByIdentifier(const char *identifier)
{
	Thread *thread = 0;

	TaskEntry *entry = runQueue;
	while (entry)
	{
		if (entry->value->alive)
		{
			const char *taskIdentifier = entry->value->getIdentifier();
			if (taskIdentifier != 0 && String::equals(taskIdentifier, identifier))
			{
				thread = entry->value;
				break;
			}
		}
		entry = entry->next;
	}

	if (!thread)
	{
		entry = waitQueue;
		while (entry)
		{
			if (entry->value->alive)
			{
				const char *taskIdentifier = entry->value->getIdentifier();
				if (taskIdentifier != 0 && String::equals(taskIdentifier, identifier))
				{
					thread = entry->value;
					break;
				}
			}
			entry = entry->next;
		}
	}

	return thread;
}

/**
 *
 */
TaskEntry *Scheduler::_removeFromQueue(TaskEntry **queueHead, Thread *thread)
{
	TaskEntry *removedEntry = 0;
	TaskEntry *entry = *queueHead;

	// if queue is empty, entry can't be removed
	if (!entry) return 0;

	// if it's the head of the queue, replace it
	if (entry->value == thread)
	{
		removedEntry = entry;
		*queueHead = removedEntry->next;
	}

	else
	{
		// otherwise, find entry before it and replace it
		TaskEntry *previous = 0;
		while (entry)
		{
			if (entry->value == thread)
			{
				removedEntry = entry;
				previous->next = removedEntry->next;
				break;
			}
			previous = entry;
			entry = entry->next;
		}
	}

	return removedEntry;
}

/**
 *
 */
void Scheduler::moveToRunQueue(Thread *thread)
{
	TaskEntry *moveEntry = _removeFromQueue(&waitQueue, thread);

	if (!moveEntry)
	{
		// entry is already in run queue
		return;
	}

	// put to start of run queue
	moveEntry->next = runQueue;
	runQueue = moveEntry;

}

/**
 *
 */
void Scheduler::moveToWaitQueue(Thread *thread)
{
	TaskEntry *moveEntry = _removeFromQueue(&runQueue, thread);

	// entry is already in wait queue
	if (!moveEntry) return;

	// put to start of wait queue
	moveEntry->next = waitQueue;
	waitQueue = moveEntry;

	// may no more be the running entry
	if (moveEntry == currentEntry) currentEntry = nullptr;

}

/**
 *
 */
void Scheduler::increaseWaitPriority(Thread *thread)
{
	// remove entry from wait queue
	TaskEntry *entry = _removeFromQueue(&waitQueue, thread);

	// put it on top of it
	if (entry)
	{
		entry->next = waitQueue;
		waitQueue = entry;
	}

}

/**
 *
 */
void Scheduler::_processWaitQueue()
{
	// process wait queue
	currentEntry = waitQueue;

	while (currentEntry)
	{
		TaskEntry *next = currentEntry->next;

		// switch to tasks space
		_applyContextSwitch(currentEntry->value);

		// remove it if its dead and check its waiting state
		if (_checkAliveState(currentEntry->value)) _checkWaitingState(currentEntry->value);

		currentEntry = next;
	}
}

/**
 *
 */
void Scheduler::_checkWaitingState(Thread *thread)
{
	// check if task must continue waiting
	if (thread->waitManager != nullptr)
	{
		if (thread->checkWaiting())
		{
			// increase wait counter for deadlock warnings
			thread->waitCount++;
			if (thread->waitCount % 500000 == 0) _printDeadlockWarning();
		}

		else
		{
			// reset wait counter & remove wait handler
			thread->waitCount = 0;
			thread->unwait();
		}
	}
}

/**
 *
 */
uint32_t Scheduler::count()
{
	uint32_t count = 0;

	TaskEntry *entry = runQueue;
	while (entry)
	{
		++count;
		entry = entry->next;
	}

	entry = waitQueue;
	while (entry)
	{
		++count;
		entry = entry->next;
	}

	return count;
}

/**
 *
 */
uint32_t Scheduler::getTaskIds(Tid *out, uint32_t len)
{
	uint32_t pos = 0;

	auto entry = runQueue;
	while (pos < len && entry)
	{
		out[pos++] = entry->value->id;
		entry = entry->next;
	}

	entry = waitQueue;
	while (pos < len && entry)
	{
		out[pos++] = entry->value->id;
		entry = entry->next;
	}

	return pos;
}

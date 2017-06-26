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

#include <tasking/tasking.hpp>

#include <EvangelionNG.hpp>
#include <logger/logger.hpp>
#include <tasking/scheduling/scheduler.hpp>
#include <system/system.hpp>
#include <tasking/ThreadManager.hpp>
#include <debug/DebugInterfaceKernel.hpp>
#include <system/ProcessorState.hpp>

static Scheduler **schedulers;

/**
 *
 */
void Tasking::initialize()
{
	// Create space for all schedulers
	uint32_t numCores = System::getNumberOfProcessors();
	schedulers = new Scheduler*[numCores];

	// Zero
	for (uint32_t i = 0; i < numCores; i++)
		schedulers[i] = 0;
}

/**
 *
 */
void Tasking::enableForThisCore()
{
	uint32_t coreId = System::currentProcessorId();
	schedulers[coreId] = new Scheduler(coreId);

	logInfo("%! scheduler installed on core %i", "tasking", coreId);
}

/**
 *
 */
void Tasking::increaseWaitPriority(Thread *thread)
{
	currentScheduler()->increaseWaitPriority(thread);
}

/**
 *
 */
Thread *Tasking::save(ProcessorState *cpuState)
{
	return currentScheduler()->save(cpuState);
}

/**
 *
 */
Thread *Tasking::schedule()
{
	return currentScheduler()->schedule();
}

/**
 *
 */
Thread *Tasking::fork(Thread *currentThread)
{
	Thread *clone = 0;

	// TODO forking in threads.
	if (currentThread == currentThread->process->main)
	{
		if (currentThread)
		{
			clone = ThreadManager::fork(currentThread);
			if (clone) addTask(clone);
		}
	}

	else logInfo("%! can't fork anything but the main thread", "todo");

	return clone;
}

/**
 *
 */
void Tasking::addTask(Thread *task, bool enforceCurrentCore)
{
	Scheduler *target = 0;

	// Used by AP's for the idle binary
	if (enforceCurrentCore) target = currentScheduler();

	else
	{
		// Find core with lowest load
		Scheduler *lowest = 0;
		uint32_t lowestLoad = 0;
		uint32_t processors = System::getNumberOfProcessors();

		for (uint32_t i = 0; i < processors; i++)
		{
			Scheduler *sched = schedulers[i];
			if (sched)
			{
				// Check if load is lower than others
				uint32_t load = sched->calculateLoad();
				if (lowest == 0 || load < lowestLoad)
				{
					lowest = sched;
					lowestLoad = load;
				}
			}
		}

		target = lowest;
	}

	// Error check
	if (target == 0) EvaKernel::panic("%! couldn't find scheduler to add task to", "tasking");

	// Assign task to scheduler
	target->add(task);
}

/**
 * Returns the current scheduler on the current core
 */
Scheduler *Tasking::currentScheduler()
{
	uint32_t coreId = System::currentProcessorId();
	Scheduler *sched = schedulers[coreId];

	// Error check
	if (sched == 0) EvaKernel::panic("%! no scheduler exists for core %i", "tasking", coreId);

	return sched;
}

/**
 *
 */
Thread *Tasking::lastThread()
{
	return currentScheduler()->lastThread();
}

/**
 *
 */
Thread *Tasking::getTaskById(uint32_t id)
{
	uint32_t processors = System::getNumberOfProcessors();

	for (uint32_t i = 0; i < processors; i++)
	{
		Scheduler *sched = schedulers[i];

		if (sched)
		{
			Thread *task = sched->getTaskById(id);
			if (task) return task;
		}
	}

	return 0;
}

/**
 *
 */
Thread *Tasking::getTaskByIdentifier(const char *identifier)
{
	uint32_t processors = System::getNumberOfProcessors();

	for (uint32_t i = 0; i < processors; i++)
	{
		Scheduler *sched = schedulers[i];

		if (sched)
		{
			Thread *task = sched->getTaskByIdentifier(identifier);
			if (task) return task;
		}
	}

	return 0;
}

/**
 *
 */
bool Tasking::registerTaskForIdentifier(Thread *task, const char *newIdentifier)
{
	// Check if someone else has this identifier
	Thread *existing = getTaskByIdentifier(newIdentifier);
	if (existing)
	{
		logWarn("%! task %i could not be registered as '%s', name is used by %i", "tasking", task->id, newIdentifier, existing->id);
		return false;
	}

	// Set the identifier
	task->setIdentifier(newIdentifier);

	DEBUG_INTERFACE_TASK_SET_IDENTIFIER(task->id, newIdentifier);
	logDebug("%! task %i registered as '%s'", "tasking", task->id, newIdentifier);
	return true;
}

/**
 *
 */
void Tasking::removeThreads(Process *process)
{
	uint32_t processors = System::getNumberOfProcessors();
	for (uint32_t i = 0; i < processors; i++)
	{
		Scheduler *scheduler = schedulers[i];
		if (scheduler) scheduler->removeThreads(process);
	}
}

/**
 *
 */
uint32_t Tasking::count()
{
	uint32_t total = 0;
	uint32_t processors = System::getNumberOfProcessors();
	for (uint32_t i = 0; i < processors; i++)
	{
		Scheduler *scheduler = schedulers[i];
		if (schedulers[i]) total = total + scheduler->count();
	}
	return total;
}

/**
 *
 */
uint32_t Tasking::getTaskIds(Tid *out, uint32_t len)
{
	uint32_t pos = 0;

	uint32_t processors = System::getNumberOfProcessors();
	for (uint32_t i = 0; i < processors; i++)
	{
		Scheduler *sched = schedulers[i];
		if (sched) pos += sched->getTaskIds(&out[pos], len - pos);
	}

	return pos;
}

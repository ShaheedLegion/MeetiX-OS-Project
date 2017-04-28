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

#include "tasking/thread.hpp"
#include "tasking/process.hpp"
#include "utils/string.hpp"
#include "kernel.hpp"
#include "logger/logger.hpp"
#include "tasking/wait/waiter_perform_interruption.hpp"
#include "debug/debug_interface_kernel.hpp"

static Tid taskIdCounter = 0;

/**
 *
 */
Thread::Thread(ThreadType _type) 
{

	id = taskIdCounter++;
	type = _type;
	priority = THREAD_PRIORITY_NORMAL;

	waitCount = 0;
	rounds = 0;
	scheduler = 0;

	alive = true;
	cpuState = 0;
	identifier = 0;

	process = 0;

	userThreadAddr = 0;
	userData = 0;
	threadEntry = 0;

	tlsCopyVirt = 0;

	userStackAreaStart = 0;
	kernelStackPageVirt = 0;
	kernelStackEsp0 = 0;

	waitManager = 0;

	interruptionInfo = 0;

	if (type == THREAD_TYPE_VM86) 
	{
		vm86Information = new ThreadInformationVm86();
	} 

	else 
	{
		vm86Information = 0;
	}
}

/**
 *
 */
Thread::~Thread() 
{

	if (identifier) 
	{
		delete identifier;
	}

	if (waitManager) 
	{
		delete waitManager;
	}

	if (vm86Information) 
	{
		delete vm86Information;
	}

	if (interruptionInfo) 
	{
		delete interruptionInfo;
	}
}

/**
 *
 */
void Thread::setIdentifier(const char *newIdentifier) 
{

	if (identifier) delete identifier;

	uint32_t identLen = String::length(newIdentifier);
	identifier = new char[identLen + 1];
	
	Memory::copy(identifier, newIdentifier, identLen);
	identifier[identLen] = 0;

	DEBUG_INTERFACE_TASK_SET_IDENTIFIER(this->id, identifier);
}

/**
 *
 */
const char *Thread::getIdentifier() 
{
	return identifier;
}

/**
 *
 */
ThreadInformationVm86 *Thread::getVm86Information() 
{
	if (type == THREAD_TYPE_VM86 && vm86Information) 
	{
		return vm86Information;
	}

	EvaKernel::panic("tried to retrieve vm86 information from the non-vm86-process type task %i", id);
	return 0; // not reached
}

/**
 *
 */
bool Thread::checkWaiting() 
{
	return waitManager->checkWaiting(this);
}

/**
 *
 */
void Thread::wait(Waiter *newWaitManager) 
{
	// replace waiter
	if (waitManager) delete waitManager;

	else scheduler->moveToWaitQueue(this);

	waitManager = newWaitManager;
	DEBUG_INTERFACE_TASK_SET_STATUS(this->id, newWaitManager->debugName());
}

/**
 *
 */
void Thread::unwait() 
{

	DEBUG_INTERFACE_TASK_SET_STATUS(this->id, "normal");
	if (waitManager) 
	{
		delete waitManager;
		waitManager = 0;
		scheduler->moveToRunQueue(this);
	}
}

/**
 *
 */
bool Thread::startPrepareInterruption() 
{

	// don't try to interrupt twice
	if (interruptionInfo != nullptr) 
	{
		return false;
	}

	// create interruption info struct and store the waiter
	interruptionInfo = new ThreadInterruptionInfo();
	interruptionInfo->waitManager = waitManager;

	return true;
}

/**
 *
 */
void Thread::finishPrepareInterruption(uintptr_t address, uintptr_t callback) 
{

	// append the waiter that does interruption
	waitManager = 0;
	wait(new WaiterPerformInterruption(address, callback));

	// the next time this thread is regularly scheduled, the waiter
	// will store the state and do interruption
}

/**
 *
 */
void Thread::enterIrqHandler(uintptr_t address, uint8_t irq, uintptr_t callback) 
{

	if (!startPrepareInterruption()) 
	{
		return;
	}

	// tell interruption info that it's about an irq
	interruptionInfo->type = ThreadInterruptionInfoType::IRQ;
	interruptionInfo->handledIrq = irq;

	DEBUG_INTERFACE_TASK_SET_STATUS(this->id, "irq-handling");
	finishPrepareInterruption(address, callback);
}

/**
 *
 */
void Thread::enterSignalHandler(uintptr_t address, int signal, uintptr_t callback) 
{

	if (!startPrepareInterruption()) 
	{
		return;
	}

	// tell interruption info that it's about an irq
	interruptionInfo->type = ThreadInterruptionInfoType::SIGNAL;
	interruptionInfo->handledSignal = signal;

	DEBUG_INTERFACE_TASK_SET_STATUS(this->id, "signal-handling");
	finishPrepareInterruption(address, callback);
}

/**
 *
 */
void Thread::storeForInterruption() 
{

	// store CPU state & it's stack location
	interruptionInfo->cpuState = *cpuState;
	interruptionInfo->cpuStateAddress = cpuState;

}

/**
 *
 */
void Thread::restoreInterruptedState() 
{

	// set the waiter that was on the thread before interruption
	wait(interruptionInfo->waitManager);

	// restore CPU state
	cpuState = interruptionInfo->cpuStateAddress;
	*cpuState = interruptionInfo->cpuState;

	// remove interruption info
	delete interruptionInfo;
	interruptionInfo = 0;
	DEBUG_INTERFACE_TASK_SET_STATUS(this->id, "quitting-interruption");
}

/**
 *
 */
void Thread::raiseSignal(int signal) 
{

	// get handler from process
	SignalHandler *handler = &(process->signalHandlers[signal]);
	if (handler->handler) 
	{

		// get the thread that handles the signal (take this if its the right one)
		Thread *handlingThread = 0;
		if (handler->threadID == this->id) 
		{
			handlingThread = this;
		} 

		else 
		{
			handlingThread = Tasking::getTaskById(handler->threadID);
		}

		// let handling thread enter signal handler
		if (handlingThread) 
		{
			handlingThread->enterSignalHandler(handler->handler, signal, handler->callback);
		}
	} 

	else 
	{

		// do default handling if no handler is registered
		bool kill = false;

		if (signal == SIGSEGV) 
		{
			kill = true;
			DEBUG_INTERFACE_TASK_SET_STATUS(this->id, "sigsegv");
		}

		if (kill) 
		{
			logInfo("%! thread %i killed", "signal", id);
			alive = false;
			process->main->alive = false;
		}
	}

}

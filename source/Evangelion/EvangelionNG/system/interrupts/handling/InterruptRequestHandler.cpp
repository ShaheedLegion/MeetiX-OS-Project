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

#include <system/interrupts/handling/InterruptRequestHandler.hpp>
#include <logger/logger.hpp>
#include <system/IOPorts.hpp>
#include <tasking/tasking.hpp>
#include <system/interrupts/pic.hpp>
#include <calls/SyscallHandler.hpp>
#include <system/system.hpp>
#include <system/interrupts/IoapicManager.hpp>
#include <memory/AddressSpace.hpp>
#include <system/interrupts/ioapic.hpp>
#include <system/interrupts/IoapicManager.hpp>

#include <tasking/tasking.hpp>
#include <tasking/ThreadManager.hpp>
#include <tasking/wait/WaiterJoin.hpp>

/**
 * Map denoting which IRQs have happened and should be handled.
 */
bool irqsWaiting[256] = { };
IrqHandler *handlers[256] = { };

/**
 * Performs interrupt request handling.
 */
Thread *InterruptRequestHandler::handle(Thread *currentThread)
{
	const uint32_t irq = currentThread->cpuState->intr - 0x20;

	if (currentThread->cpuState->intr == 0x80)
	{
		/*
		 System call
		 ===========
		 If the interrupt request is a system call, its handled by the system
		 call handler.
		 */
		currentThread = SysCallHandler::handle(currentThread);
	}

	else if (irq == 0xFF)
	{
		/**
		 * Spurious interrupt
		 */
		logWarn("%! spurious interrupt was caught", "requests");
	}

	else if (irq == 0)
	{
		/*
		 Timer request
		 =============
		 Scheduler is called. The milliseconds are updated separately, because the
		 method to schedule tasks may be called arbitrarily and is not guaranteed to be
		 called only on IRQ 0.
		 */
		Tasking::currentScheduler()->updateMilliseconds();
		currentThread = Tasking::schedule();
	}

	else if (irq < 256)
	{
		/*
		 IRQ handling
		 ============
		 These requests are stored in the map of waiting IRQs. The IRQ is masked so it
		 doesnt happen again before it has been handled.
		 */
		IrqHandler *handler = handlers[irq];
		if (handler)
		{
			Thread *thread = Tasking::getTaskById(handler->threadID);
			if (thread != nullptr)
			{
				// let the thread enter the irq handler
				thread->enterIrqHandler(handler->handler, irq, handler->callback);
				// it could be the current thread, so we have to switch
				currentThread = Tasking::schedule();
			}
		}

		else
		{
			// Mark the IRQ and mask it
			irqsWaiting[irq] = true;
		}

		// TODO this dies in VMWare: IOAPICManager::maskIrq(irq);
	}

	else
	{
		logWarn("%! unhandled irq %i", "requests", irq);
	}

	return currentThread;
}

/**
 *
 */
bool InterruptRequestHandler::pollIrq(uint8_t irq)
{
	if (irqsWaiting[irq])
	{

		irqsWaiting[irq] = false;

		// TODO this dies in VMWare: IOAPICManager::unmaskIrq(irq);

		return true;
	}

	return false;
}

/**
 *
 */
void InterruptRequestHandler::setHandler(uint8_t irq, Tid threadID, uintptr_t handlerAddr, uintptr_t callbackAddr)
{
	if (handlers[irq]) delete handlers[irq];

	IrqHandler *handler = new IrqHandler;
	handler->threadID = threadID;
	handler->handler = handlerAddr;
	handler->callback = callbackAddr;
	handlers[irq] = handler;
}

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

#include <tasking/wait/waiter_perform_interruption.hpp>

/**
 *
 */
bool WaiterPerformInterruption::checkWaiting(Thread *task) 
{

	task->storeForInterruption();

	// set the new entry
	task->cpuState->eip = entry;

	// jump to next stack value
	uint32_t* esp = (uint32_t*) (task->cpuState->esp);

	// pass parameter value
	if (task->interruptionInfo->type == ThreadInterruptionInfoType::IRQ) 
	{
		// pass IRQ
		--esp;
		*esp = task->interruptionInfo->handledIrq;

	} 

	else if (task->interruptionInfo->type == ThreadInterruptionInfoType::SIGNAL) 
	{
		// pass signal
		--esp;
		*esp = task->interruptionInfo->handledSignal;
	}

	// put callback as return address on stack
	--esp;
	*esp = callback;

	// set new ESP
	task->cpuState->esp = (uint32_t) esp;

	return false;
}


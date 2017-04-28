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

#ifndef EVA_MULTITASKING_WAIT_MANAGER_INTERRUPTS
#define EVA_MULTITASKING_WAIT_MANAGER_INTERRUPTS

#include <tasking/wait/waiter.hpp>
#include <system/interrupts/handling/interrupt_request_handler.hpp>

/**
 *
 */
class WaiterWaitForIrq : public Waiter 
{
private:
	uint32_t interrupt;

public:
	WaiterWaitForIrq(uint32_t _interrupt) 
	{
		this->interrupt = _interrupt;
	}

	/**
	 *
	 */
	virtual bool checkWaiting(Thread *task) 
	{
		bool fired = InterruptRequestHandler::pollIrq((uint32_t) interrupt);

		if (fired) 
		{
			// Interrupt was fired
			return false;
		} 

		else 
		{
			// Keep waiting for the interrupt
			return true;
		}
	}

	/**
	 *
	 */
	virtual const char *debugName() 
	{
		return "waitForIrq";
	}

};

#endif

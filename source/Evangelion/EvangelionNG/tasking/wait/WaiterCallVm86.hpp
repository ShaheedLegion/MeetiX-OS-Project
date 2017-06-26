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

#ifndef EVA_MULTITASKING_WAIT_MANAGER_VIRTUAL_8086
#define EVA_MULTITASKING_WAIT_MANAGER_VIRTUAL_8086

#include "eva/kernel.h"
#include <tasking/wait/waiter.hpp>

/**
 *
 */
class WaiterCallVm86 : public Waiter
{
private:
	SyscallCallVm86 *data;
	Vm86Registers *temporaryOut;
	uint32_t virtual8086ProcessId;

public:
	WaiterCallVm86(SyscallCallVm86 *_data, Vm86Registers *_temporaryOut, uint32_t _virtual8086ProcessId) : data(_data), temporaryOut(_temporaryOut), virtual8086ProcessId(_virtual8086ProcessId) {}

	/**
	 *
	 */
	virtual bool checkWaiting(Thread *task)
	{
		Thread *vm86task = Tasking::getTaskById(virtual8086ProcessId);

		if (vm86task == 0)
		{
			// Task has finished, copy the values to the waiting data struct
			*(data->out) = *temporaryOut;

			// Delete temporary out struct
			delete temporaryOut;

			return false;
		}

		// Keep waiting for Vm86 to finish
		else return true;

	}

	/**
	 *
	 */
	virtual const char *debugName()
	{
		return "WaiterCallVm86";
	}

};

#endif

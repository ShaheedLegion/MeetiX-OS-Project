/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __INTERFACE_COMMAND_APPLICATION_EXIT_CLEANUP_THREAD__
#define __INTERFACE_COMMAND_APPLICATION_EXIT_CLEANUP_THREAD__

#include <mxuser/tasking/thread.hpp>
#include <interface/command_message_receiver_thread.hpp>

/**
 *
 */
class ApplicationExitCleanupThread_t: public Thread 
{
private:
	Tid pid;
	CommandMessageReceiverThread_t *commandReceiver;

public:
	/**
	 *
	 */
	ApplicationExitCleanupThread_t(Pid pid, CommandMessageReceiverThread_t *commandReceiver) : pid(pid), commandReceiver(commandReceiver) 
	{
	}

	/**
	 *
	 */
	virtual void run();

};

#endif

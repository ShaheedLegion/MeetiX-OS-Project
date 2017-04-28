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

#include <interface/command_message_responder_thread.hpp>
#include <mxuser/ui/interface_specification.hpp>
#include <stdio.h>
#include <zipNET.hpp>
#include <events/event_processor.hpp>
#include <eva.h>

/**
 *
 */
void CommandMessageResponderThread_t::run() 
{
	TaskRegisterID("zipNET/CommandMessageResponderThread");
	EventProcessor_t *eventProcessor = ZipNET::instance()->eventProcessor;
	while (true) 
	{

		// wait until messages are added
		AtomicLock(&bufferEmpty);

		// process all
		while (buffer.size() > 0) 
		{

			// get reference to response from the queue
			CommandMessageResponse_t &response = buffer.back();
			SendMessageT(response.target, response.message, response.length, response.transaction);

			// delete message buffer
			delete (MessageHeader*) response.message;

			// remove response from queue
			buffer.pop_back();
		}
	}
}

/**
 *
 */
void CommandMessageResponderThread_t::sendResponse(CommandMessageResponse_t &response) 
{
	buffer.push_back(response);
	bufferEmpty = false;
}


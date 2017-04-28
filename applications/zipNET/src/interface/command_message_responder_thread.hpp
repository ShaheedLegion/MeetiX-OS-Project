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

#ifndef __INTERFACE_COMMAND_MESSAGE_RESPONDER_THREAD__
#define __INTERFACE_COMMAND_MESSAGE_RESPONDER_THREAD__

#include <mxuser/tasking/thread.hpp>
#include <deque>

/**
 *
 */
typedef struct 
{
	Tid target;
	uint32_t transaction;
	void *message;
	size_t length;
} CommandMessageResponse_t;

/**
 *
 */
class CommandMessageResponderThread_t : public Thread 
{
public:
	std::deque<CommandMessageResponse_t> buffer;
	uint8_t bufferEmpty = true;

	/**
	 *
	 */
	virtual void run();

	/**
	 *
	 */
	void sendResponse(CommandMessageResponse_t &response);

};

#endif

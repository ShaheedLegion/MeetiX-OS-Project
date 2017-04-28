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

#include "tasking/wait/waiter_receive_message.hpp"
#include "tasking/communication/message_controller.hpp"
#include "logger/logger.hpp"

/**
 *
 */
bool WaiterReceiveMessage::checkWaiting(Thread *task) 
{

	// if break is issued, stop sleeping
	if (data->breakCondition != nullptr && *data->breakCondition) 
	{
		data->status = MESSAGE_RECEIVE_STATUS_INTERRUPTED;
		return false;
	}

	data->status = MessageController::receiveMessage(task->id, data->buffer, data->maximum, data->transaction);

	// if queue is empty, continue sleeping
	if (data->status == MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY) 
	{
		return true;
	}

	// stop sleeping
	return false;
}


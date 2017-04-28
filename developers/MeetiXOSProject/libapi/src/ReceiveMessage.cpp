/*********************************************************************************
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]                  *
* 																			     *
* This program is free software; you can redistribute it and/or                  *
* modify it under the terms of the GNU General Public License                    *
* as published by the Free Software Foundation; either version 2				 *
* of the License, or (char *argumentat your option) any later version.			 *
*																				 *
* This program is distributed in the hope that it will be useful,				 *
* but WITHout ANY WARRANTY; without even the implied warranty of                 *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 				 *
* GNU General Public License for more details.									 *
*																				 *
* You should have received a copy of the GNU General Public License				 *
* along with this program; if not, write to the Free Software                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
**********************************************************************************/

#include "eva/user.h"

// redirect
MessageReceiveStatus ReceiveMessage(void *buf, size_t max)
{
	return ReceiveMessageTMB(buf, max, MESSAGE_TRANSACTION_NONE, MESSAGE_RECEIVE_MODE_BLOCKING, nullptr);
}

// redirect
MessageReceiveStatus ReceiveMessageM(void *buf, size_t max, MessageReceiveMode mode)
{
	return ReceiveMessageTMB(buf, max, MESSAGE_TRANSACTION_NONE, mode, nullptr);
}

// redirect
MessageReceiveStatus ReceiveMessageT(void *buf, size_t max, MessageTransaction tx)
{
	return ReceiveMessageTMB(buf, max, tx, MESSAGE_RECEIVE_MODE_BLOCKING, nullptr);
}

// redirect
MessageReceiveStatus ReceiveMessageTM(void *buf, size_t max, MessageTransaction tx, MessageReceiveMode mode)
{
	return ReceiveMessageTMB(buf, max, tx, mode, nullptr);
}

/**
 *
 */
MessageReceiveStatus ReceiveMessageTMB(void *buf, size_t max, MessageTransaction tx, MessageReceiveMode mode, uint8_t *breakCondition)
{

	SyscallReceiveMessage data;
	data.buffer = (MessageHeader*) buf;
	data.maximum = max;
	data.mode = mode;
	data.transaction = tx;
	data.breakCondition = breakCondition;
	
	syscall(SYSCALL_MESSAGE_RECEIVE, (uint32_t) &data);
	
	return data.status;
}

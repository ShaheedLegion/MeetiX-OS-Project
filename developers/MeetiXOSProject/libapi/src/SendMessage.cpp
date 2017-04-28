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
MessageSendStatus SendMessage(Tid target, void *buf, size_t len)
{
	return SendMessageTM(target, buf, len, MESSAGE_TRANSACTION_NONE, MESSAGE_SEND_MODE_BLOCKING);
}

/**
 *
 */
MessageSendStatus SendMessageM(Tid target, void *buf, size_t len, MessageSendMode mode)
{
	return SendMessageTM(target, buf, len, MESSAGE_TRANSACTION_NONE, mode);
}

/**
 *
 */
MessageSendStatus SendMessageT(Tid tid, void *buf, size_t len, MessageTransaction tx)
{
	return SendMessageTM(tid, buf, len, tx, MESSAGE_SEND_MODE_BLOCKING);
}

/**
 *
 */
MessageSendStatus SendMessageTM(Tid tid, void *buf, size_t len, MessageTransaction tx, MessageSendMode mode)
{
	SyscallSendMessage data;
	data.buffer = buf;
	data.length = len;
	data.receiver = tid;
	data.mode = mode;
	data.transaction = tx;
	
	syscall(SYSCALL_MESSAGE_SEND, (uint32_t) &data);
	
	return data.status;
}


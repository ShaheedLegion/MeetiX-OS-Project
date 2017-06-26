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

#include <tasking/communication/MessageController.hpp>
#include <logger/logger.hpp>
#include "utils/HashMap.hpp"
#include "memory/memory.hpp"
#include "memory/collections/AddressStack.hpp"

typedef HashMap<Tid, MessageQueueHead*> MessageQueueMap;
static MessageQueueMap *queues = 0;

static AddressStack messageMemoryPool32;
static AddressStack messageMemoryPool64;
static AddressStack messageMemoryPool256;
static AddressStack messageMemoryPool2048;
static AddressStack messageMemoryPool4096;
static AddressStack messageMemoryPool8192;

/**
 *
 */
void refill(AddressStack *pool, size_t size)
{
	for (int i = 0; i < 32; i++)
	{
		Address addr = (Address) new uint8_t[size];
		if (!addr)
		{
			logInfo("%! out of memory while refilling memory pool %i", "messages", size);
			break;
		}
		pool->push(addr);
	}
}

/**
 *
 */
Address getFromPool(AddressStack *pool, size_t size)
{
	Address addr = pool->pop();

	if (!addr)
	{
		refill(pool, size);
		return getFromPool(pool, size);
	}

	return addr;
}

/**
 *
 */
MessageHeader *get(size_t size)
{
	if (size < 32) return (MessageHeader*) getFromPool(&messageMemoryPool32, 32);
	else if (size < 64) return (MessageHeader*) getFromPool(&messageMemoryPool64, 64);
	else if (size < 256) return (MessageHeader*) getFromPool(&messageMemoryPool256, 256);
	else if (size < 2048) return (MessageHeader*) getFromPool(&messageMemoryPool2048, 2048);
	else if (size < 4096)return (MessageHeader*) getFromPool(&messageMemoryPool4096, 4096);
	else if (size < 8192)return (MessageHeader*) getFromPool(&messageMemoryPool8192, 8192);

	logInfo("%! invalid fill-pool requested, size %i", "messages", size);
	return 0;
}

/**
 *
 */
void release(MessageHeader *m)
{
	size_t size = sizeof(MessageHeader) + m->length;

	if (size < 32) messageMemoryPool32.push((Address) m);
	else if (size < 64) messageMemoryPool64.push((Address) m);
	else if (size < 256) messageMemoryPool256.push((Address) m);
	else if (size < 2048) messageMemoryPool2048.push((Address) m);
	else if (size < 4096)messageMemoryPool4096.push((Address) m);
	else if (size < 8192)messageMemoryPool8192.push((Address) m);
	else logInfo("%! invalid released requested, size %i", "messages", size);
}

/**
 *
 */
void MessageController::clear(Tid tid)
{
	if (!queues) return;

	// get the queue
	auto entry = queues->get(tid);
	if (entry == nullptr) return;

	// release all messages
	MessageQueueHead *head = entry->value;
	MessageHeader *m = head->first;
	while (m)
	{
		MessageHeader *after = m->next;
		release(m);
		m = after;
	}

	// release the queue
	queues->erase(tid);
	delete head;
}

/**
 *
 */
MessageSendStatus MessageController::sendMessage(Tid target, Tid source, void *content, size_t contentLen, MessageTransaction tx)
{
	// ensure queue map
	if (!queues) queues = new MessageQueueMap();

	// check if message too long
	if (contentLen > MESSAGE_MAXIMUM_LENGTH) return MESSAGE_SEND_STATUS_EXCEEDS_MAXIMUM;

	// find/create queue head
	MessageQueueHead *queue;

	MessageQueueMap::HashMapEntry *entry = queues->get(target);
	if (entry == nullptr)
	{
		queue = new MessageQueueHead();
		queues->add(target, queue);
	}

	else queue = entry->value;

	// check if it exceeds queue maximum
	if (queue->total + contentLen > MESSAGE_MAXIMUM_QUEUE_CONTENT) return MESSAGE_SEND_STATUS_QUEUE_FULL;

	// create message
	MessageHeader *message = get(sizeof(MessageHeader) + contentLen);
	if (!message) return MESSAGE_RECEIVE_STATUS_FAILED;
	message->transaction = tx;
	message->sender = source;
	message->length = contentLen;
	Memory::copy(MESSAGE_CONTENT(message), content, contentLen);

	// append to queue
	if (queue->first == nullptr) queue->first = message;
	if (queue->last) queue->last->next = message;
	message->previous = queue->last;
	message->next = nullptr;
	queue->last = message;

	// increment queue total content length
	queue->total += contentLen;

	return MESSAGE_SEND_STATUS_SUCCESSFUL;
}

/**
 *
 */
MessageReceiveStatus MessageController::receiveMessage(Tid target, MessageHeader *out, size_t max, MessageTransaction tx)
{
	// check for map
	if (!queues) return MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY;

	// find queue head
	MessageQueueHead *queue;

	MessageQueueMap::HashMapEntry *entry = queues->get(target);
	if (!entry) return MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY;
	else queue = entry->value;

	if (!queue) return MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY;

	// find message
	MessageHeader *message = nullptr;

	if (tx == MESSAGE_TRANSACTION_NONE) message = queue->first;

	else
	{
		MessageHeader *n = queue->first;
		while (n)
		{
			if (n->transaction == tx)
			{
				message = n;
				break;
			}
			n = n->next;
		}
	}

	// no message?
	if (!message) return MESSAGE_RECEIVE_STATUS_QUEUE_EMPTY;

	// check if message exceeds bounds
	size_t contentLen = message->length;
	if ((sizeof(MessageHeader) + contentLen) > max) return MESSAGE_RECEIVE_STATUS_EXCEEDS_BUFFER_SIZE;

	// copy message
	Memory::copy(out, message, sizeof(MessageHeader) + contentLen);

	// erase from queue
	if (message->next) message->next->previous = message->previous;
	else
	{
		queue->last = message->previous;
		if (queue->last) queue->last->next = 0;
	}

	if (message->previous) message->previous->next = message->next;
	else
	{
		queue->first = message->next;
		if (queue->first) queue->first->previous = 0;
	}

	// free the message
	release(message);

	// shrink queue total content length
	queue->total -= contentLen;

	//logInfo("%! message received by thread: %i", "recmes", target);
	return MESSAGE_RECEIVE_STATUS_SUCCESSFUL;
}

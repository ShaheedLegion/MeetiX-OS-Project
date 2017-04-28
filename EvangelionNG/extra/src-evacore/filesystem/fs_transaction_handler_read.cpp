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

#include "filesystem/fs_delegate.hpp"
#include "filesystem/fs_transaction_handler_read.hpp"
#include "filesystem/filesystem.hpp"
#include "logger/logger.hpp"
#include "tasking/wait/waiter_fs_transaction.hpp"

/**
 *
 */
FsTransactionHandlerStartStatus FsTransactionHandlerRead::startTransaction(Thread *thread) 
{

	// create a context-bound wrapper for the data buffer
	Contextual<uint8_t*> boundBuffer(data()->buffer, thread->process->pageDirectory);

	// check for the driver delegate
	FsDelegate *delegate = node->getDelegate();
	if (delegate == 0) 
	{
		logWarn("%! reading of '%i' failed due to missing delegate on underlying node %i", "filesystem", fd->id, node->id);
		return FS_TRANSACTION_START_FAILED;
	}

	// check if the transaction is repeated only
	if (wantsRepeatTransaction()) 
	{
		// when a transaction is repeated, the waiter is still on the requesters task
		delegate->requestRead(thread, node, data()->length, boundBuffer, fd, this);
		return FS_TRANSACTION_START_WITH_WAITER;
	}

	// start transaction by requesting the delegate
	FsTransactionID transaction = delegate->requestRead(thread, node, data()->length, boundBuffer, fd, this);

	// check status for possible immediate finish
	if (WaiterFsTransaction::isTransactionWaiting(thread, this, transaction, delegate)) 
	{
		// otherwise append waiter
		thread->wait(new WaiterFsTransaction(this, transaction, delegate));
		return FS_TRANSACTION_START_WITH_WAITER;
	}

	return FS_TRANSACTION_START_IMMEDIATE_FINISH;

}

/**
 *
 */
FsTransactionHandlerFinishStatus FsTransactionHandlerRead::finishTransaction(Thread *thread, FsDelegate *delegate) 
{
	delegate->finishRead(thread, &status, &result, fd);

	data()->result = result;
	data()->status = status;
	return FS_TRANSACTION_HANDLING_DONE;
}
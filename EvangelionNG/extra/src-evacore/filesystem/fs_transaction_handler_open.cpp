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
#include "filesystem/fs_transaction_handler_open.hpp"
#include "filesystem/filesystem.hpp"
#include "logger/logger.hpp"
#include "tasking/wait/waiter_fs_transaction.hpp"

/**
 *
 */
FsTransactionHandlerStartStatus FsTransactionHandlerOpen::startTransaction(Thread *thread) 
{

	// check for the driver delegate
	FsDelegate *delegate = node->getDelegate();
	if (delegate == 0) 
	{
		logWarn("%! failed to open node %i due to missing delegate", "filesystem", node->id);
		return FS_TRANSACTION_START_FAILED;
	}

	// start transaction by requesting the delegate
	FsTransactionID transaction = delegate->requestOpen(thread, node, data()->path, data()->flags, data()->mode, this);

	// check status for possible immediate finish
	if (WaiterFsTransaction::isTransactionWaiting(thread, this, transaction, delegate)) 
	{
		thread->wait(new WaiterFsTransaction(this, transaction, delegate));
		return FS_TRANSACTION_START_WITH_WAITER;
	}

	return FS_TRANSACTION_START_IMMEDIATE_FINISH;
}

/**
 *
 */
FsTransactionHandlerFinishStatus FsTransactionHandlerOpen::finishTransaction(Thread *thread, FsDelegate *delegate) 
{

	delegate->finishOpen(thread, this);

	data()->status = status;

	// when delegate says opening is fine, create the file descriptor
	if (status == FS_OPEN_SUCCESSFUL) 
	{
		data()->fd = FileSystem::mapFile(thread->process->main->id, node, data()->flags);
	} 

	else 
	{
		data()->fd = -1;
	}

	return FS_TRANSACTION_HANDLING_DONE;
}

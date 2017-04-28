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
#include "filesystem/fs_transaction_handler_directory_refresh.hpp"
#include "tasking/wait/waiter_fs_transaction.hpp"
#include "logger/logger.hpp"

/**
 *
 */
FsTransactionHandlerStartStatus FsTransactionHandlerDirectoryRefresh::startTransaction(Thread *thread)
{

	// take the delegate
	FsDelegate *delegate = folder->getDelegate();
	if (delegate == nullptr) 
	{
		data()->status = FS_READ_DIRECTORY_ERROR;
		logWarn("%! reading directory failed due to missing delegate on node %i", "filesystem", folder->id);
		return FS_TRANSACTION_START_FAILED;
	}

	// request the refresh
	FsTransactionID transaction = delegate->requestDirectoryRefresh(thread, folder, this);
	thread->wait(new WaiterFsTransaction(this, transaction, delegate));
	return FS_TRANSACTION_START_WITH_WAITER;
}

/**
 *
 */
FsTransactionHandlerFinishStatus FsTransactionHandlerDirectoryRefresh::finishTransaction(Thread *thread, FsDelegate *delegate)
{

	if (delegate) 
	{
		delegate->finishDirectoryRefresh(thread, this);
	}

	if (unfinishedHandler) 
	{
		return unfinishedHandler->finishTransaction(thread, delegate);
	}

	return FS_TRANSACTION_HANDLING_DONE;
}

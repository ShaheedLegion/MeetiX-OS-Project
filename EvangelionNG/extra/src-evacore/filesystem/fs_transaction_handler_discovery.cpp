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

#include "filesystem/fs_transaction_handler_discovery.hpp"
#include "filesystem/fs_delegate.hpp"
#include "filesystem/filesystem.hpp"
#include "eva/utils/local.hpp"
#include "tasking/wait/waiter_fs_transaction.hpp"
#include "logger/logger.hpp"

/**
 *
 */
FsTransactionHandlerStartStatus FsTransactionHandlerDiscovery::startTransaction(Thread *thread)
{

	// check if this node is already discovered
	FsNode *parent = 0;
	FsNode *child = 0;
	Local<char> lastName(new char[PATH_MAX]);
	FileSystem::findExisting(absolutePath, &parent, &child, lastName(), followSymlinks);

	// if the node already exists, tell the handler that discovery was successful
	if (child) 
	{
		status = FS_DISCOVERY_SUCCESSFUL;
		node = child;
		allNodesDiscovered = true;
		finishTransaction(thread, child->getDelegate());
		return FS_TRANSACTION_START_IMMEDIATE_FINISH;
	}

	// otherwise, request the driver delegate to discover it and set to sleep
	FsDelegate *delegate = parent->getDelegate();
	if (delegate == nullptr) 
	{
		// this is an error error
		if (parent == FileSystem::getRoot()) 
		{
			logWarn("%! mountpoint for '%s' does not exist", "filesystem", absolutePath);
		} 

		else 
		{
			logWarn("%! discovery of '%s' failed due to missing delegate on node %i", "filesystem", absolutePath, parent->id);
		}
		status = FS_DISCOVERY_ERROR;
		allNodesDiscovered = true;
		finishTransaction(thread, 0);
		return FS_TRANSACTION_START_FAILED;
	}

	// set the last discovered parent, so that the open command knows the last existing parent
	lastDiscoveredParent = parent;

	// start the discovery
	FsTransactionID transaction = delegate->requestDiscovery(thread, parent, lastName(), this);
	thread->wait(new WaiterFsTransaction(this, transaction, delegate));
	return FS_TRANSACTION_START_WITH_WAITER;
}

/**
 *
 */
FsTransactionHandlerFinishStatus FsTransactionHandlerDiscovery::finishTransaction(Thread *thread, FsDelegate *delegate)
{

	// if not all nodes have been discovered yet, we need to do a loop
	if (!allNodesDiscovered) 
	{
		// allow the delegate to finish
		if (delegate) 
		{
			delegate->finishDiscovery(thread, this);
		}

		// if discovering the next node was successful, go on with discovering
		if (status == FS_DISCOVERY_SUCCESSFUL) 
		{
			// continue discovery
			FsTransactionHandlerStartStatus status = startTransaction(thread);

			// check for possible failure
			if (status == FS_TRANSACTION_START_FAILED) 
			{
				return FS_TRANSACTION_HANDLING_DONE;
			} 

			else 
			{
				// check if all nodes have been discovered now
				if (!allNodesDiscovered) 
				{
					return FS_TRANSACTION_HANDLING_REPEAT_WITH_SAME_HANDLER;
				}
			}
		}
	}

	// when everything is done, perform afterwork; might want to keep waiting again
	return afterFinishTransaction(thread);
}

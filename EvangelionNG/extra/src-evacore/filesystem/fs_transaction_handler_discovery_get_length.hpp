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

#ifndef EVA_FILESYSTEM_TRANSACTION_HANDLER_DISCOVERY_GET_LENGTH
#define EVA_FILESYSTEM_TRANSACTION_HANDLER_DISCOVERY_GET_LENGTH

#include "filesystem/fs_transaction_handler_discovery.hpp"
#include "filesystem/fs_node.hpp"
#include "filesystem/fs_descriptors.hpp"
#include "memory/contextual.hpp"
#include "logger/logger.hpp"

/**
 *
 */
class FsTransactionHandlerDiscoveryGetLength : public FsTransactionHandlerDiscovery 
{
public:
	Contextual<SyscallFsLength*> data;

	/**
	 *
	 */
	FsTransactionHandlerDiscoveryGetLength(char *absolutePathIn, bool followSymlinks, Contextual<SyscallFsLength*> data) : FsTransactionHandlerDiscovery(absolutePathIn, followSymlinks), data(data) 
	{
	}

	/**
	 *
	 */
	virtual FsTransactionHandlerFinishStatus afterFinishTransaction(Thread *thread) 
	{

		if (status == FS_DISCOVERY_SUCCESSFUL) 
		{
			// create and start the new handler
			FsTransactionHandlerGetLengthDefault *handler = new FsTransactionHandlerGetLengthDefault(data, node);
			auto startStatus = handler->startTransaction(thread);

			if (startStatus == FS_TRANSACTION_START_WITH_WAITER) 
			{
				return FS_TRANSACTION_HANDLING_KEEP_WAITING_WITH_NEW_HANDLER;

			} 

			else if (startStatus == FS_TRANSACTION_START_IMMEDIATE_FINISH) 
			{
				return FS_TRANSACTION_HANDLING_DONE;

			} 

			else 
			{
				data()->status = FS_LENGTH_ERROR;
				logWarn("%! failed to start get-length handler after node discovery", "filesystem");
				return FS_TRANSACTION_HANDLING_DONE;
			}

		} 

		else 
		{
			data()->length = -1;
			data()->status = FS_LENGTH_ERROR;
			return FS_TRANSACTION_HANDLING_DONE;
		}
	}

};

#endif

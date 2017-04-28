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

#ifndef EVA_FILESYSTEM_TRANSACTION_HANDLER_DISCOVERY_OPEN_DIRECTORY
#define EVA_FILESYSTEM_TRANSACTION_HANDLER_DISCOVERY_OPEN_DIRECTORY

#include "filesystem/fs_transaction_handler_discovery.hpp"
#include "filesystem/fs_node.hpp"
#include "filesystem/fs_descriptors.hpp"
#include "memory/contextual.hpp"
#include "logger/logger.hpp"

/**
 *
 */
class FsTransactionHandlerDiscoveryOpenDirectory: public FsTransactionHandlerDiscovery 
{
public:
	Contextual<SyscallFsOpenDirectory*> data;

	/**
	 *
	 */
	FsTransactionHandlerDiscoveryOpenDirectory(char *absolutePathIn, Contextual<SyscallFsOpenDirectory*> data) : FsTransactionHandlerDiscovery(absolutePathIn), data(data) 
	{
	}

	/**
	 *
	 */
	virtual FsTransactionHandlerFinishStatus afterFinishTransaction(Thread *thread) 
	{

		if (status == FS_DISCOVERY_SUCCESSFUL) 
		{
			// fill the call iterator with the node id & reset position
			data()->iterator->nodeID = node->id;
			data()->iterator->position = 0;
			data()->status = FS_OPEN_DIRECTORY_SUCCESSFUL;

		} 

		else if (status == FS_DISCOVERY_NOT_FOUND) 
		{
			data()->status = FS_OPEN_DIRECTORY_NOT_FOUND;

		} 

		else if (status == FS_DISCOVERY_ERROR || status == FS_DISCOVERY_BUSY) 
		{
			data()->status = FS_OPEN_DIRECTORY_ERROR;
		}

		return FS_TRANSACTION_HANDLING_DONE;
	}

};

#endif
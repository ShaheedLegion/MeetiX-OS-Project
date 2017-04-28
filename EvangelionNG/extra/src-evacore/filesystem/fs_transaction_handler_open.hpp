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

#ifndef EVA_FILESYSTEM_TRANSACTION_HANDLER_OPEN
#define EVA_FILESYSTEM_TRANSACTION_HANDLER_OPEN

#include "filesystem/fs_transaction_handler.hpp"
#include "filesystem/fs_node.hpp"
#include "filesystem/fs_descriptors.hpp"
#include "memory/contextual.hpp"

/**
 *
 */
class FsTransactionHandlerOpen : public FsTransactionHandler 
{
public:
	Contextual<SyscallFsOpen*> data;
	FsDiscoveryStatus discoveryStatus;
	FsNode *node;

	FsOpenStatus status = FS_OPEN_ERROR;

	/**
	 * @param data
	 * 		system call data structure
	 * @param discovery_status
	 * 		status of the preceding discovery operation
	 * @param node
	 * 		discovered node to use; must be the actual node if the node was found,
	 * 		otherwise the last discovered parent
	 */
	FsTransactionHandlerOpen(Contextual<SyscallFsOpen*> data, FsDiscoveryStatus discoveryStatus, FsNode *node) : data(data), discoveryStatus(discoveryStatus), node(node) 
	{
	}

	/**
	 *
	 */
	virtual FsTransactionHandlerStartStatus startTransaction(Thread *thread);

	/**
	 *
	 */
	virtual FsTransactionHandlerFinishStatus finishTransaction(Thread *thread, FsDelegate *delegate);

};

#endif

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

#ifndef EVA_FILESYSTEM_TRANSACTION_HANDLER_READ_DIRECTORY
#define EVA_FILESYSTEM_TRANSACTION_HANDLER_READ_DIRECTORY

#include "filesystem/fs_transaction_handler.hpp"
#include "filesystem/fs_transaction_handler_directory_refresh.hpp"
#include "filesystem/fs_node.hpp"
#include "filesystem/fs_descriptors.hpp"
#include "memory/contextual.hpp"

/**
 *
 */
class FsTransactionHandlerReadDirectory : public FsTransactionHandler 
{
public:
	FsNode *folder;
	Contextual<SyscallFsReadDirectory*> data;

	FsTransactionHandlerDirectoryRefresh *causingHandler = 0;

	/**
	 *
	 */
	FsTransactionHandlerReadDirectory(FsNode *folder, Contextual<SyscallFsReadDirectory*> data) : folder(folder), data(data) 
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

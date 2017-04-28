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

#ifndef EVA_FILESYSTEM_FILESYSTEM
#define EVA_FILESYSTEM_FILESYSTEM

#include "filesystem/fs_delegate.hpp"
#include "filesystem/fs_node.hpp"
#include "filesystem/fs_transaction_handler.hpp"
#include "filesystem/fs_transaction_handler_read.hpp"
#include "filesystem/fs_transaction_handler_read_directory.hpp"
#include "filesystem/fs_transaction_handler_write.hpp"
#include "filesystem/fs_transaction_handler_discovery.hpp"
#include "filesystem/fs_transaction_handler_get_length.hpp"

#include "eva/stdint.h"
#include <tasking/tasking.hpp>
#include "debug/debug_interface_kernel.hpp"

/**
 *
 */
class FileSystem 
{
public:

	/**
	 * Initializes the filesystem, allocating & preparing all necessary data structures
	 * that are used.
	 */
	static void initialize();

	/**
	 *
	 */
	static FsNode *getRoot();

	/**
	 *
	 */
	static FsNode *getNodeById(FsVirtID id);

	/**
	 * Creates a new filesystem node, assigning a unique node id and
	 * adding it to the global map of nodes.
	 *
	 * @return the created node
	 */
	static FsNode *createNode();

	/**
	 * Tries to resolve the node with the given absolute path. The buffer behind
	 * "name_current" is filled with the name of the last found node (must be of at
	 * least G_PATH_MAX bytes size).
	 *
	 * - If the node is found, parent and child are set
	 * - If the node is NOT found, parent is set, the child is set to 0
	 */
	static void findExisting(char *absolutePath, FsNode **outParent, FsNode **outChild, char *nameCurrent, bool followSymlinks = true);

	/**
	 * Resolves the real path to the given node and writes it to the out buffer.
	 *
	 * @param node
	 * 		the node to retrieve the path for
	 *
	 * @param out
	 * 		the output buffer with a size of at least {G_PATH_MAX} bytes
	 */
	static void getRealPathToNode(FsNode *node, char *out);

	/**
	 * Tries to create a mountpoint with the given name and registers the given thread
	 * as the delegate.
	 *
	 * @param thread
	 * 		the thread that is used as the delegate
	 *
	 * @param name
	 * 		name for the mountpoint
	 *
	 * @param phys_mountpoint_id
	 * 		the physical mountpoint id to send the delegate when discovering children
	 *
	 * @param out_mountpoint_id
	 * 		is filled with the generated mountpoint id on success
	 *
	 * @param out_transaction_storage
	 * 		is filled with the address of the transaction storage on success
	 *
	 * @return one of the {FsRegisterAsDelegateStatus} codes
	 */
	static FsRegisterAsDelegateStatus createDelegate(Thread *thread, char *name, FsPhysID physMountpointID, FsVirtID *outMountpointID, Address *outTransactionStorage);

	static bool nodeForDescriptor(Pid pid, Fd fd, FsNode **outNode, FileDescriptorContent **outFd);

	/**
	 * If the in path is absolute (starts with a '/') it is simply copied to the out buffer.
	 * If the in path is a relative path, it is appended to the working directory (adding
	 * a '/' in between) and written to the out buffer.
	 *
	 * @param relative_base
	 * 		the relative base to append on relative paths
	 *
	 * @param in
	 * 		the absolute or relative path
	 *
	 * @param out
	 * 		the output buffer of at least {G_PATH_MAX} bytes size
	 */
	static void concatAsAbsolutePath(char *relativeBase, char *in, char *out);

	/**
	 * Opens a file, creating a file descriptor for the given node within
	 * the given process.
	 *
	 * @param pid
	 * 		id of the process
	 *
	 * @param node
	 * 		the node to open
	 *
	 * @param flags
	 * 		open mode flags
	 *
	 * @param-opt fd
	 * 		the file descriptor to use
	 *
	 * @return a file descriptor
	 */
	static Fd mapFile(Pid pid, FsNode *node, int32_t openFlags, Fd fd = -1);

	/**
	 *
	 */
	static bool unmapFile(Pid pid, FsNode *node, FileDescriptorContent *fd);

	/**
	 *
	 */
	static Fd clonefd(Fd sourceFd, Pid sourcePid, Fd targetFd, Pid targetPid, FsClonefdStatus *outStatus);

	/**
	 *
	 */
	static FsPipeStatus pipe(Thread *thread, Fd *outWrite, Fd *outRead);

	/**
	 *
	 */
	static void processClosed(Pid pid);

	/**
	 *
	 */
	static void processForked(Pid source, Pid fork);

};

#endif

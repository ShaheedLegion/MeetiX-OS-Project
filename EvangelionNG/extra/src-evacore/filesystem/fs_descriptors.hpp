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

#ifndef EVA_FILESYSTEM_FILEDESCRIPTORS
#define EVA_FILESYSTEM_FILEDESCRIPTORS

#include "eva/fs.h"
#include "filesystem/pipes.hpp"
#include <utils/hash_map.hpp>
#include <tasking/process.hpp>

/**
 *
 */
struct FileDescriptorContent 
{
	Fd id;
	int64_t offset;
	FsVirtID nodeID;
	int32_t openFlags;

	void cloneInto(FileDescriptorContent *other) 
	{
		other->offset = offset;
		other->nodeID = nodeID;
		other->openFlags = openFlags;
	}
};

/**
 *
 */
struct FileDescriptorTable 
{
	Fd nextFileDescriptor = 3; // skips stdin/stdout/stderr
	HashMap<Fd, FileDescriptorContent*> descriptors;
};

/**
 *
 */
class FileDescriptors 
{
private:
	static FileDescriptorContent *createDescriptor(FileDescriptorTable *table, Fd overrideFd = -1);

public:
	static void initialize();

	static Fd map(Pid pid, FsVirtID nodeID, Fd fd, int32_t openFlags);
	static void unmap(Pid pid, Fd fd);
	static void unmapAll(Pid pid);

	static FileDescriptorContent *get(Pid pid, Fd fd);
	static FileDescriptorTable *getProcessTable(Pid pid);
};

#endif

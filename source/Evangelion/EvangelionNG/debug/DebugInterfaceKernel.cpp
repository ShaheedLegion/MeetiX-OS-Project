/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <debug/DebugInterfaceKernel.hpp>
#include "system/serial/SerialPort.hpp"

// only required in full interface mode
#if DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_FULL

/**
 *
 */
void EvaDebugInterface::publishTaskStatus(int tid, const char *status)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED;

	DebugInterface::writeShort(DEBUG_MESSAGE_TASK_SET_STATUS);
	DebugInterface::writeInt(tid);
	DebugInterface::writeString(status);
}

/**
 *
 */
void EvaDebugInterface::publishTaskRounds(int tid, long rounds)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED;

	DebugInterface::writeShort(DEBUG_MESSAGE_TASK_SET_ROUNDS);
	DebugInterface::writeInt(tid);
	DebugInterface::writeLong(rounds);
}

/**
 *
 */
void EvaDebugInterface::taskSetIdentifier(int tid, const char *identifier)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED

	DebugInterface::writeShort(DEBUG_MESSAGE_TASK_SET_IDENTIFIER);
	DebugInterface::writeInt(tid);
	DebugInterface::writeString(identifier);
}

/**
 *
 */
void EvaDebugInterface::taskSetSourcePath(int tid, const char *sourcePath)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED

	DebugInterface::writeShort(DEBUG_MESSAGE_TASK_SET_SOURCE_PATH);
	DebugInterface::writeInt(tid);
	DebugInterface::writeString(sourcePath);
}

/**
 *
 */
void EvaDebugInterface::filesystemUpdateNode(FsNode *node)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED

	DebugInterface::writeShort(DEBUG_MESSAGE_FILESYSTEM_UPDATE_NODE);
	DebugInterface::writeInt(node->id);
	DebugInterface::writeInt(node->phys_fs_id);
	DebugInterface::writeInt(node->parent != 0 ? node->parent->id : -1);
	DebugInterface::writeString((const char*) node->name);
}

/**
 *
 */
void EvaDebugInterface::memorySetPageUsage(uint32_t pageaddr, int usage)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED

	DebugInterface::writeShort(DEBUG_MESSAGE_MEMORY_SET_PAGE_USAGE);
	DebugInterface::writeInt(pageaddr);
	DebugInterface::writeByte(usage);
}

/**
 *
 */
void EvaDebugInterface::systemInformation(const char *key, uint64_t value)
{
	DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED

	DebugInterface::writeShort(DEBUG_MESSAGE_SYSTEM_INFORMATION);
	DebugInterface::writeString(key);
	DebugInterface::writeLong(value);
}

#endif

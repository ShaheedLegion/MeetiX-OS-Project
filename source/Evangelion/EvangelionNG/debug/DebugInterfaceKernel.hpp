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

#ifndef __EVA_DEBUG_INTERFACE_KERNEL__
#define __EVA_DEBUG_INTERFACE_KERNEL__

#include "eva/stdint.h"
#include "debug/DebugInterface.hpp"
#include "filesystem/filesystem.hpp"

#define DEBUG_INTERFACE_EXIT_IF_UNINITIALIZED						if (!DebugInterfaceInitialized) return;

// shortcuts
#if DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_FULL
#define DEBUG_INTERFACE_FILESYSTEM_UPDATE_NODE(node)				EvaDebugInterface::filesystemUpdateNode(node);
#define DEBUG_INTERFACE_MEMORY_SET_PAGE_USAGE(pageaddr, usage)		EvaDebugInterface::memorySetPageUsage(pageaddr, usage);
#define DEBUG_INTERFACE_TASK_SET_IDENTIFIER(task, identifier)		EvaDebugInterface::taskSetIdentifier(task, identifier);
#define DEBUG_INTERFACE_TASK_SET_STATUS(task, status)				EvaDebugInterface::publishTaskStatus(task, status);
#define DEBUG_INTERFACE_TASK_SET_ROUNDS(task, rounds)				EvaDebugInterface::publishTaskRounds(task, rounds);
#define DEBUG_INTERFACE_TASK_SET_SOURCE_PATH(task, source_path)		EvaDebugInterface::taskSetSourcePath(task, source_path);
#define DEBUG_INTERFACE_SYSTEM_INFORMATION(key, value)				EvaDebugInterface::systemInformation(key, value);

#else
#define DEBUG_INTERFACE_FILESYSTEM_UPDATE_NODE(node)
#define DEBUG_INTERFACE_MEMORY_SET_PAGE_USAGE(pageaddr, usage)
#define DEBUG_INTERFACE_TASK_SET_IDENTIFIER(task, identifier)
#define DEBUG_INTERFACE_TASK_SET_STATUS(task, status)
#define DEBUG_INTERFACE_TASK_SET_SOURCE_PATH(task, source_path)
#define DEBUG_INTERFACE_SYSTEM_INFORMATION(key, value)

#endif

/**
 *
 */
class EvaDebugInterface
{
public:
	/**
	 *
	 */
	static void publishTaskStatus(int tid, const char *status);
	/**
	 *
	 */
	static void publishTaskRounds(int tid, long rounds);

	/**
	 *
	 */
	static void taskSetIdentifier(int tid, const char *identifier);

	/**
	 *
	 */
	static void taskSetSourcePath(int tid, const char *sourcePath);

	/**
	 *
	 */
	static void filesystemUpdateNode(FsNode *node);

	/**
	 *
	 */
	static void memorySetPageUsage(uint32_t addr, int usage);

	/**
	 *
	 */
	static void systemInformation(const char *key, uint64_t value);

};

#endif

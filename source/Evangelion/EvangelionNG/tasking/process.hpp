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

#ifndef EVA_MULTITASKING_PROCESS_
#define EVA_MULTITASKING_PROCESS_

#include "eva/types.h"
#include <tasking/thread.hpp>
#include <utils/ListEntry.hpp>
#include <memory/collections/AddressRangePool.hpp>
#include <system/smp/GlobalLock.hpp>

/**
 * Constants used as flags on virtual ranges of processes
 */
#define PROC_VIRTUAL_RANGE_FLAG_NONE						0
#define PROC_VIRTUAL_RANGE_FLAG_PHYSICAL_OWNER				1

/**
 *
 */
struct SignalHandler
{
public:
	uintptr_t handler = 0;
	uintptr_t callback = 0;
	Tid threadID = 0;
};

/**
 *
 */
class Process
{
public:
	// process that execute this
	Process *parent;

	// main thread of process
	Thread *main;

	// Make sure to adjust forking when changing this.
	VirtualAddress imageStart;
	VirtualAddress imageEnd;    // TODO
	VirtualAddress heapStart;   // Maybe use some kind of "SectionList" here
	VirtualAddress heapBreak;
	uint32_t heapPages;

	// Taken from virtual ranges:
	VirtualAddress tlsMasterInProcLocation;
	VirtualAddress tlsMasterCopysize;
	VirtualAddress tlsMasterTotalsize;
	VirtualAddress tlsMasterAlignment;

	PageDirectory pageDirectory;
	char *cliArguments;
	char *workingDirectory;
	char *sourcePath;

	AddressRangePool virtualRanges;

	SecurityLevel securityLevel;

	SignalHandler signalHandlers[SIG_COUNT];

	/*
	 *
	 */
	void setPath(const char *path);

	/**
	 *
	 */
	Process(SecurityLevel securityLevel);
	~Process();

};

#endif

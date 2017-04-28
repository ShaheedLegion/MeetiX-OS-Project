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

#ifndef EVA_MULTITASKING_PROCESS_
#define EVA_MULTITASKING_PROCESS_

#include "eva/types.h"
#include <tasking/thread.hpp>
#include <utils/list_entry.hpp>
#include <memory/collections/address_range_pool.hpp>
#include <system/smp/global_lock.hpp>

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
	Process *parent;
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

	/**
	 *
	 */
	Process(SecurityLevel securityLevel);
	~Process();

};

#endif

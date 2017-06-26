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

#include <memory/AddressSpace.hpp>
#include <memory/paging.hpp>
#include <memory/memory.hpp>
#include <memory/physical/PPallocator.hpp>
#include <memory/TemporaryPagingUtil.hpp>
#include <memory/constants.hpp>
#include <tasking/tasking.hpp>

#include <EvangelionNG.hpp>
#include <logger/logger.hpp>

/**
 * Creates a mapping from the virtualAddress to the physicalAddress. Writes the entries
 * to the recursively mapped directory in the last 4MB of the memory.
 */
bool AddressSpace::map(VirtualAddress virtualAddr, PhysicalAddress physicalAddr, uint32_t tableFlags, uint32_t pageFlags, bool allowOverride)
{
	// check if addresses are aligned
	if ((virtualAddr & PAGE_ALIGN_MASK) || (physicalAddr & PAGE_ALIGN_MASK)) EvaKernel::panic("%! tried to map unaligned addresses: virt %h to phys %h", "addrspace", virtualAddr, physicalAddr);

	// calculate table & page indices
	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(virtualAddr);
	uint32_t pi = PAGE_IN_TABLE_INDEX(virtualAddr);

	// get pointers to directory and table
	PageDirectory directory = (PageDirectory) CONST_RECURSIVE_PAGE_DIRECTORY_ADDRESS;
	PageTable table = ((PageTable) CONST_RECURSIVE_PAGE_DIRECTORY_AREA) + (0x400 * ti);

	// create table if it does not exist
	if (directory[ti] == 0)
	{
		PhysicalAddress newTablePhys = PPallocator::allocate();
		if (newTablePhys == 0)
			EvaKernel::panic("%! no pages left for mapping", "addrspace");

		// insert table
		directory[ti] = newTablePhys | tableFlags;

		// empty the created (and mapped) table
		for (uint32_t i = 0; i < 1024; i++)
			table[i] = 0;

		logDebug("%! created table %i", "addrspace", ti);
	}

	else
	{
		// this is illegal and an unrecoverable error
		if (tableFlags & PAGE_TABLE_USERSPACE)
			if (((directory[ti] & PAGE_ALIGN_MASK) & PAGE_TABLE_USERSPACE) == 0)
				EvaKernel::panic("%! tried to map user page in kernel space table, virt %h", "addrspace", virtualAddr);
	}

	// put address into table
	if (table[pi] == 0 || allowOverride)
	{
		table[pi] = physicalAddr | pageFlags;

		// flush address
		INVLPG(virtualAddr);
		return true;
	}

	else
	{
		Thread *failor = Tasking::lastThread();
		if (failor != 0)
		{
			const char *ident = failor->getIdentifier();
			if (ident)
			{
				logInfo("%! '%s' (%i) tried duplicate mapping, virt %h -> phys %h, table contains %h", "addrspace", ident, failor->id, virtualAddr, physicalAddr, table[pi]);
			}

			else
			{
				logInfo("%! %i tried duplicate mapping, virt %h -> phys %h, table contains %h", "addrspace", failor->id, virtualAddr, physicalAddr, table[pi]);
			}
		}

		else
		{
			logInfo("%! unknown tried duplicate mapping, virt %h -> phys %h, table contains %h", "addrspace", virtualAddr, physicalAddr, table[pi]);
		}
	}
	return false;
}

/**
 *
 */
void AddressSpace::mapToTemporaryMappedDirectory(PageDirectory directory, VirtualAddress virtualAddr, PhysicalAddress physicalAddr, uint32_t tableFlags, uint32_t pageFlags, bool allowOverride)
{
	// check if addresses are aligned
	if ((virtualAddr & PAGE_ALIGN_MASK) || (physicalAddr & PAGE_ALIGN_MASK)) EvaKernel::panic("%! tried to map unaligned addresses: virt %h to phys %h", "addrspace", virtualAddr, physicalAddr);

	// calculate table & page indices
	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(virtualAddr);
	uint32_t pi = PAGE_IN_TABLE_INDEX(virtualAddr);

	// create table if it does not exist
	if (directory[ti] == 0)
	{
		PhysicalAddress newTablePhys = PPallocator::allocate();
		if (newTablePhys == 0) EvaKernel::panic("%! no pages left for mapping", "addrspace");

		// temporary map the table and insert it
		VirtualAddress tempTableAddr = TemporaryPagingUtil::map(newTablePhys);
		PageTable table = (PageTable) tempTableAddr;
		for (uint32_t i = 0; i < 1024; i++)
			table[i] = 0;

		TemporaryPagingUtil::unmap(tempTableAddr);

		// insert table
		directory[ti] = newTablePhys | tableFlags;
		logDebug("%! created table %i while mapping %h - %h (temp)", "addrspace", ti, virtualAddr, physicalAddr);
	}

	// Insert address into table
	PhysicalAddress tablePhys = (directory[ti] & ~PAGE_ALIGN_MASK);

	VirtualAddress tempTableAddr = TemporaryPagingUtil::map(tablePhys);
	PageTable table = (PageTable) tempTableAddr;
	if (table[pi] == 0 || allowOverride) table[pi] = physicalAddr | pageFlags;

	else
	{
		logWarn("%! tried to map area to virtual pd %h that was already mapped, virt %h -> phys %h, table contains %h", "addrspace", directory, virtualAddr, physicalAddr, table[pi]);

		EvaKernel::panic("%! duplicate mapping", "addrspace");
	}
	TemporaryPagingUtil::unmap(tempTableAddr);
}

/**
 *
 */
void AddressSpace::unmap(VirtualAddress virtualAddress)
{
	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(virtualAddress);
	uint32_t pi = PAGE_IN_TABLE_INDEX(virtualAddress);

	PageDirectory directory = (PageDirectory) CONST_RECURSIVE_PAGE_DIRECTORY_ADDRESS;
	PageTable table = CONST_RECURSIVE_PAGE_TABLE(ti);

	if (directory[ti] == 0) return;

	// Remove address from table
	if (table[pi] != 0)
	{
		table[pi] = 0;

		// Flush address
		INVLPG(virtualAddress);
	}
}

/**
 * Switches to the given page-directory
 */
void AddressSpace::switchToSpace(PageDirectory directory)
{
	asm volatile("mov %0, %%cr3":: "b"(directory));
}

/**
 * Returns the currenty page directory
 */
PageDirectory AddressSpace::getCurrentSpace()
{
	uint32_t directory;
	asm volatile("mov %%cr3, %0" : "=r"(directory));
	return (PageDirectory) directory;
}

/**
 *
 */
PhysicalAddress AddressSpace::virtualToPhysical(VirtualAddress addr)
{
	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(addr);
	uint32_t pi = PAGE_IN_TABLE_INDEX(addr);

	PageTable table = CONST_RECURSIVE_PAGE_TABLE(ti);

	if (table == 0) return 0;

	return table[pi] & ~PAGE_ALIGN_MASK;
}

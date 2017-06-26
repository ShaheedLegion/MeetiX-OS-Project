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

#ifndef EVA_MEMORY_ADDRESS_SPACE
#define EVA_MEMORY_ADDRESS_SPACE

#include "eva/stdint.h"
#include <memory/paging.hpp>
#include <memory/memory.hpp>
#include <memory/AddressSpace.hpp>

/**
 * Functionality to manipulate the address space.
 */
class AddressSpace
{
public:

	/**
	 * Maps a page to the current address space.
	 *
	 * @param virt
	 * 		the virtual address to map to
	 * @param phys
	 * 		the address of the physical page to map
	 * @param table_flags
	 * 		the flags to add on the table entry
	 * @param page_flags
	 * 		the flags to add on the page entry
	 * @param allow_override
	 * 		whether an existing entry may be overriden
	 *
	 *
	 */
	static bool map(VirtualAddress virt, PhysicalAddress phys, uint32_t tableFlags, uint32_t pageFlags, bool allowOverride = false);

	/**
	 * Maps a page to a page directory that is temporarily mapped into the current address space.
	 *
	 * @param directory
	 * 		the directory to map into
	 * @other-params see map
	 */
	static void mapToTemporaryMappedDirectory(PageDirectory directory, VirtualAddress virtualAddress, PhysicalAddress physicalAddress, uint32_t tableFlags, uint32_t pageFlags, bool allowOverride = false);

	/**
	 * Unmaps the given virtual page in the current address space.
	 *
	 * @param virt
	 * 		the virtual address to unmap
	 */
	static void unmap(VirtualAddress virt);

	/**
	 * Switches to the given page directory.
	 *
	 * @param dir
	 * 		the directory to switch to
	 */
	static void switchToSpace(PageDirectory dir);

	/**
	 * Returns the currently set page directory.
	 *
	 * @return the page directory
	 */
	static PageDirectory getCurrentSpace();

	/**
	 * Reads for a given virtual address (which must exist in the currently mapped
	 * address space) the underlying physical address.
	 *
	 * @param addr
	 * 		the address to resolve
	 *
	 * @return the physical address
	 */
	static PhysicalAddress virtualToPhysical(VirtualAddress addr);

};

#endif

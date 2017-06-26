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

#ifndef EVA_MEMORY_ADDRESS_RANGE_POOL
#define EVA_MEMORY_ADDRESS_RANGE_POOL

#include <memory/memory.hpp>

/**
 * An address range is a range of pages starting at a base. The base
 * determines the address of the range, and pages is the number of
 * pages the range has.
 */
struct AddressRange
{
	AddressRange() : next(0), used(false), base(0), pages(0), flags(0) {}

	AddressRange *next;

	bool used;
	Address base;
	uint32_t pages;

	uint8_t flags;
};

/**
 * A address range pool manages ranges of page-aligned virtual
 * addresses.
 */
class AddressRangePool
{
private:
	AddressRange *first;

public:
	AddressRangePool() : first(0) {}

	~AddressRangePool();

	Address allocate(uint32_t pages, uint8_t flags = 0);
	int32_t free(Address base);

	AddressRange *getRanges();

	/**
	 * Initialize from ranges
	 */
	void initialize(Address start, Address end);

	/**
	 * Initialize by clone
	 */
	void initialize(AddressRangePool *other);

	void dump(bool onlyFree = false);

private:
	void merge();
};

#endif

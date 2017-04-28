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

#include <memory/collections/address_range_pool.hpp>
#include <memory/paging.hpp>
#include <logger/logger.hpp>

/**
 * Inserts the addresses in the given range as free addresses in the pool.
 * The start and end should be page-aligned.
 */
void AddressRangePool::initialize(Address start, Address end) 
{
	AddressRange *newRange = new AddressRange;
	newRange->base = start;
	newRange->used = false;
	newRange->pages = (end - start) / PAGE_SIZE;

	if (first == 0) 
	{
		// If the pool is empty, add it as first
		newRange->next = 0;
		first = newRange;
	} 

	else 
	{
		// Find the last range that has a lower base than this one
		AddressRange *lastBelow = 0;
		AddressRange *range = first;
		do 
		{
			if (range->base < newRange->base) lastBelow = range;
		} 
		while ((range = range->next) != 0);

		if (lastBelow) 
		{
			// Place the new range between the lastBelow and the one after it
			newRange->next = lastBelow->next;
			lastBelow->next = newRange;
		} 

		else 
		{
			// There was no range found that has a lower base than this,
			// so add it as first
			newRange->next = first;
			first = newRange;
		}
	}

	merge();
}

/**
 * Clones all ranges
 */
void AddressRangePool::initialize(AddressRangePool *other) 
{
	AddressRange *otherCurrent = (AddressRange*) other->first;
	AddressRange *last = 0;
	do 
	{
		AddressRange *newRange = new AddressRange;
		*newRange = *otherCurrent;
		newRange->next = 0;

		if (last) 
		{
			last->next = newRange;
			last = newRange;
		} 

		else 
		{
			first = newRange;
			last = newRange;
		}
	} 
	while ((otherCurrent = otherCurrent->next) != 0);
}

/**
 * Deletes all ranges.
 */
AddressRangePool::~AddressRangePool() 
{

	AddressRange *range = first;
	while (range) 
	{
		AddressRange *next = range->next;
		delete range;
		range = next;
	}
}

/**
 * Returns the linked range list.
 */
AddressRange *AddressRangePool::getRanges() 
{
	return first;
}

/**
 * Allocates a contiguous area of pages. The physicalOwner flag is by default 'true'.
 */
Address AddressRangePool::allocate(uint32_t requestedPages, uint8_t flags) 
{
	// Must have at least 1 page
	if (requestedPages == 0) 
	{
		requestedPages = 1;
	}

	// Find an unused range that has more/equal requested pages
	AddressRange *range = first;
	while (range) 
	{
		if (!range->used && range->pages >= requestedPages) break;

		range = range->next;
	}

	if (range != 0) 
	{
		// Found a range
		range->used = true;
		range->flags = flags;

		// If there are pages remaining (range is not a perfect match)
		// then we have to create a new range from the remaining space.
		int32_t remainingPages = range->pages - requestedPages;
		if (remainingPages > 0) 
		{
			AddressRange *splinter = new AddressRange;
			splinter->used = false;
			splinter->pages = remainingPages;
			splinter->base = range->base + requestedPages * PAGE_SIZE;

			// Restore linking
			splinter->next = range->next;
			range->next = splinter;
			range->pages = requestedPages;
		}

		return range->base;
	}

	logWarn("%! critical, no free range of size %i pages", "addrpool", requestedPages);
	dump();
	return 0;
}

/**
 * Frees the given range. Performs a merge afterwards.
 */
int32_t AddressRangePool::free(Address base) 
{
	int32_t freedPages = -1;

	// Look for the range with the base
	AddressRange *range = first;
	do 
	{
		if (range->base == base) break;
	} 
	while ((range = range->next) != 0);

	// Found the range
	if (range != 0) 
	{
		if (range->used == false) 
		{
			logInfo("%! bug: tried to free the unused range %h", "addrpool", range->base);
		} 

		else 
		{
			range->used = false;
			freedPages = range->pages;

			merge();
		}
	} 

	else 
	{
		logInfo("%! bug: tried to free a range (%h) that doesn't exist", "addrpool", base);
	}

	return freedPages;
}

/**
 * Merges contiguous free ranges.
 */
void AddressRangePool::merge() 
{
	AddressRange *current = (AddressRange*) first;

	while (true) 
	{
		// If it has next, continue merging
		if (current->next) 
		{

			// If (current is free) and (next is free) and (end of range == base of next) do merge
			if (!current->used && !current->next->used && ((current->base + current->pages * PAGE_SIZE) == current->next->base)) 
			{
				current->pages += current->next->pages;

				AddressRange *nextnext = current->next->next;
				delete current->next;
				current->next = nextnext;
				// Don't step to next here, check this one again for another free following block

			} 

			// Not able to merge, step to the next one
			else current = current->next;

		} 

		// End of list
		else break;
	}
}

/**
 * Dumps the ranges.
 */
void AddressRangePool::dump(bool onlyFree) 
{
	logDebug("%! range structure:", "vra");
	if (first == 0) 
	{
		logDebug("%#  cannot dump, no first entry");
		return;
	}

	AddressRange *current = (AddressRange*) first;
	do 
	{
		if (!onlyFree || !current->used) 
		{
			logDebug("%#  used: %b, base: %h, pages: %i (- %h)", current->used, current->base, current->pages, current->base + current->pages * PAGE_SIZE);
		}
	} 
	while ((current = current->next) != 0);
}


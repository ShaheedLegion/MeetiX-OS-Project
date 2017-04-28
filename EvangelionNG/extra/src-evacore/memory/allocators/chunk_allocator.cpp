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

#include <memory/allocators/chunk_allocator.hpp>
#include <logger/logger.hpp>

/**
 *
 */
void ChunkAllocator::initialize(VirtualAddress start, VirtualAddress end) 
{
	first = (ChunkHeader*) start;
	first->used = false;
	first->size = end - start - sizeof(ChunkHeader);
	first->next = 0;
}

/**
 *
 */
void ChunkAllocator::expand(VirtualAddress size) 
{

	// Find last chunk
	ChunkHeader *last = first;
	while (last->next) 
	{
		last = last->next;
	}

	// Create appendix
	ChunkHeader *appendix = (ChunkHeader*) (((uint32_t) last) + sizeof(ChunkHeader) + last->size);
	appendix->used = false;
	appendix->size = size - sizeof(ChunkHeader);
	appendix->next = 0;

	// Append on the end
	last->next = appendix;

	merge();
}

/**
 *
 */
void* ChunkAllocator::allocate(uint32_t size) 
{

	if (first == 0) 
	{
		logInfo("%! critical: tried to use allocate on uninitialized chunk allocator", "chunkalloc");
		return 0;
	}

	if (size < CHUNK_ALLOCATOR_MINIMUM_ALLOCATION_SIZE) size = CHUNK_ALLOCATOR_MINIMUM_ALLOCATION_SIZE;

	ChunkHeader *current = first;
	do 
	{
		if (!current->used && (current->size >= (size + sizeof(ChunkHeader)))) 
		{

			ChunkHeader *splinter = (ChunkHeader*) ((uint32_t) current + sizeof(ChunkHeader) + size);
			splinter->size = current->size - size - sizeof(ChunkHeader);
			splinter->used = false;
			splinter->next = current->next;

			current->next = splinter;
			current->used = true;
			current->size = size;

			return (void*) (((uint32_t) current) + sizeof(ChunkHeader));
		}
	} while ((current = current->next) != 0);

	return 0;
}

/**
 *
 */
uint32_t ChunkAllocator::free(void *mem) 
{
	if (first == 0) 
	{
		logInfo("%! critical: tried to use free on uninitialized chunk allocator", "chunkalloc");
		return 0;
	}

	ChunkHeader *blockHeader = (ChunkHeader*) (((uint32_t) mem) - sizeof(ChunkHeader));
	blockHeader->used = false;
	uint32_t size = blockHeader->size;

	merge();

	return size;
}

/**
 * Merges contiguous free blocks
 */
void ChunkAllocator::merge() 
{
	ChunkHeader *current = (ChunkHeader*) first;

	while (true) 
	{
		// If it has next, continue merging
		if (current->next) 
		{

			// If current is free and next is free, merge them
			if (!(current->used) && !(current->next->used)) 
			{
				current->size += sizeof(ChunkHeader) + current->next->size;
				current->next = current->next->next;
				// Don't step to next here, check this one again for another free following block

			} 

			// Cannot merge here, step to next
			else current = current->next;
		} 

		else break;
	}
}

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

#include <memory/lower_heap.hpp>
#include <kernel.hpp>
#include <logger/logger.hpp>
#include <memory/allocators/chunk_allocator.hpp>

static ChunkAllocator allocator;

/**
 *
 */
void LowerHeap::addArea(uint32_t start, uint32_t end) 
{
	allocator.initialize(start, end);
	logDebug("%! using area %h to %h", "lowermem", start, end);
}

/**
 *
 */
void *LowerHeap::allocate(int32_t size) 
{

	void *allocated = allocator.allocate(size);
	if (allocated) 
	{
		return allocated;
	}

	logInfo("%! out of memory", "loweralloc");
	return 0;
}

/**
 *
 */
void LowerHeap::free(void *mem) 
{
	allocator.free(mem);
}

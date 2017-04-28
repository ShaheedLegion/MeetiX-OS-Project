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

#ifndef CHUNKALLOCATOR_HPP_
#define CHUNKALLOCATOR_HPP_

#include "eva/stdint.h"
#include "eva/types.h"
#include <logger/logger.hpp>

#define CHUNK_ALLOCATOR_MINIMUM_ALLOCATION_SIZE	8

/**
 *
 */
struct ChunkHeader 
{
	ChunkHeader *next;
	uint8_t used;
	uint32_t size;
};

/**
 *
 */
class ChunkAllocator 
{
private:
	ChunkHeader *first;
	void merge();

public:
	ChunkAllocator() : first(0) {}

	void initialize(VirtualAddress start, VirtualAddress end);
	void expand(VirtualAddress size);

	void *allocate(uint32_t size);
	uint32_t free(void* memory);

};

#endif

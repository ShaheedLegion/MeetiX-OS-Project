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

#ifndef MEMORY_LOWERMEM_ALLOC
#define MEMORY_LOWERMEM_ALLOC

#include <stddef.h>
#include "eva/stdint.h"

/**
 *
 */
struct LowerHeapHeader
{
	LowerHeapHeader *next;
	uint8_t used;
	uint32_t size;
};

/**
 *
 */
class LowerHeap 
{
public:
	static void addArea(uint32_t start, uint32_t end);
	static void *allocate(int32_t size);
	static void free(void* memory);
	static void merge();

	static void dump();
};

#endif
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

#include <memory/bitmap/bitmap_page_allocator.hpp>

#include <multiboot/multiboot.hpp>
#include <memory/paging.hpp>

#include <logger/logger.hpp>

/**
 * 
 */
void BitMapPageAllocator::initialize(BitMapEntry* theBitmap) 
{

	bitmap = theBitmap;

	logDebug("%! initializing to bitmap %h", "bitmap", bitmap);
	for (uint32_t i = 0; i < BITMAP_LENGTH; i++) 
	{
		bitmap[i] = 0;
	}
	logDebug("%! map zeroed", "physical");
}

/**
 * 
 */
void BitMapPageAllocator::markFree(uint32_t address) 
{
	uint32_t index = ADDRESS_TO_BITMAP_INDEX(address);
	uint32_t bit = ADDRESS_TO_BITMAP_BIT(address);

	BITMAP_SET(bitmap, index, bit);
}

/**
 * 
 */
uint32_t BitMapPageAllocator::allocate() 
{

	for (uint32_t i = 0; i < BITMAP_LENGTH; i++) 
	{
		// If the entry is more than zero there is at least one free
		if (bitmap[i] > 0) 
		{

			for (uint32_t b = 0; b < BITMAP_BITS_PER_ENTRY; b++) 
			{
				// If this bit is set, this page is free
				if (BITMAP_IS_SET(bitmap, i, b)) 
				{
					BITMAP_UNSET(bitmap, i, b);

					// Page found, clear if necessary and return
					return BITMAP_TO_ADDRESS(i, b);
				}
			}
		}
	}

	return 0;
}

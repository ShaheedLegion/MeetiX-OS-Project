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

#include <memory/memory.hpp>

/**
 * 
 */
void *Memory::setBytes(void *target, uint8_t value, int32_t length) 
{

	uint8_t* pos = (uint8_t*) target;
	while (length--) 
	{
		*pos++ = (uint8_t) value;
	}
	return target;
}

/**
 * 
 */
void *Memory::setWords(void *target, uint16_t value, int32_t length) 
{

	uint16_t* pos = (uint16_t*) target;
	while (length--) 
	{
		*pos++ = (uint16_t) value;
	}
	return target;
}

/**
 * 
 */
void *Memory::copy(void *target, const void *source, int32_t length) 
{

	uint8_t *targetPos = (uint8_t*) target;
	const uint8_t *sourcePos = (const uint8_t*) source;

	while (length--) 
	{
		*targetPos++ = *sourcePos++;
	}

	return target;
}

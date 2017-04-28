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

#ifndef __EVA_SHARED_SYSTEM_IOPORTS__
#define __EVA_SHARED_SYSTEM_IOPORTS__

#include "eva/stdint.h"

/**
 *
 */
class IOports 
{
public:
	static uint8_t readByte(uint16_t port);
	static void writeByte(uint16_t port, uint8_t data);
	static uint16_t readShort(uint16_t port);
	static void writeShort(uint16_t port, uint16_t data);
	static uint32_t readInt(uint16_t port);
	static void writeInt(uint16_t port, uint32_t data);
};

#endif

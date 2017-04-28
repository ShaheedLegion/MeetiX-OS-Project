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

#include <system/io_ports.hpp>

/**
 * Reads a byte from the given CPU port
 */
uint8_t IOports::readByte(uint16_t port) 
{
	uint8_t value;
	asm volatile("inb %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

/**
 * Writes the given data to the given CPU port
 */
void IOports::writeByte(uint16_t port, uint8_t value) 
{
	asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

/**
 * Reads a short from the given CPU port
 */
uint16_t IOports::readShort(uint16_t port) 
{
	uint16_t value;
	asm volatile("inw %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

/**
 * Writes the given data to the given CPU port
 */
void IOports::writeShort(uint16_t port, uint16_t value) 
{
	asm volatile("outw %1, %0" : : "dN" (port), "a" (value));
}

/**
 * Reads a dword from the given CPU port
 */
uint32_t IOports::readInt(uint16_t port) 
{
	uint32_t value;
	asm volatile("inl %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

/**
 * Writes the given data to the given CPU port
 */
void IOports::writeInt(uint16_t port, uint32_t value) 
{
	asm volatile("outl %1, %0" : : "dN" (port), "a" (value));
}


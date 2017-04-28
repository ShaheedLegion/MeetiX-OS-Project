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

#ifndef __EVA_DEBUG_INTERFACE__
#define __EVA_DEBUG_INTERFACE__

#include "eva/stdint.h"
#include "debug/debug_protocol.hpp"
#include "debug/debug_interface_mode.hpp"
#include "build_config.hpp"

// determines whether the interface is initialized
extern bool DebugInterfaceInitialized;

/**
 *
 */
class DebugInterface
{
public:

	/**
	 *
	 */
	static void initialize(uint16_t port);

	/**
	 *
	 */
	static void writeLogCharacter(char c);

	/**
	 *
	 */
	static void writeByte(uint8_t value);

	/**
	 *
	 */
	static void writeShort(uint16_t value);

	/**
	 *
	 */
	static void writeInt(uint32_t value);

	/**
	 *
	 */
	static void writeLong(uint64_t value);

	/**
	 *
	 */
	static void writeString(const char* string);

};

#endif

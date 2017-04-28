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

#ifndef EVA_LOADER_MULTIBOOT_MULTIBOOTMODULEANALYZER
#define EVA_LOADER_MULTIBOOT_MULTIBOOTMODULEANALYZER

#include <multiboot/multiboot.hpp>
#include "eva/stdint.h"

/**
 * Multiboot module analyzation util
 */
class MultibootModuleAnalyzer 
{
public:

	/**
	 * Calculates the end of the highest module in memory.
	 *
	 * @return the address of the highest module in memory
	 */
	static uint32_t calculateEndOfHighestModule();

};

#endif

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

#ifndef EVA_LOADER_LOADER
#define EVA_LOADER_LOADER

#include <multiboot/multiboot.hpp>
#include "eva/stdint.h"
#include <kernelloader/setup_information.hpp>
#include <memory/memory.hpp>
#include <memory/bitmap/bitmap_page_allocator.hpp>

/**
 * The loader class is responsible for preparing the virtual memory management
 * and then loading the kernel to higher memory.
 */
class EvaLoader 
{
public:

	/**
	 * This function is used to find a chunk of free memory even before any memory
	 * management was initialized. It is used to find a place to put the GDT and the
	 * bitmap, to avoid accidentally overwriting multiboot modules.
	 */
	static uint32_t findFreeMemory(MultibootInformation* info, uint32_t start, int count);

	/**
	 *
	 */
	static BitMapPageAllocator *getPhysicalAllocator();

	/**
	 * Initializes the system by preparing the necessities to enable virtual memory and
	 * then loads the kernel module to the higher half and jumps to it.
	 *
	 * @param multibootInformation	the GRUB multiboot information
	 */
	static void initialize(MultibootInformation* multibootInformation);

	/**
	 * Triggers a loader panic, means halting the entire system and displaying the given
	 * message/diagnostic information.
	 *
	 * @param message	the panic message, written in a format that the g_logger class understands
	 * @param ...		variable arguments for the message
	 */
	static void panic(const char *msg, ...);

	/**
	 * @return the loaders setup information struct
	 */
	static SetupInformation* getSetupInformation();
};

#endif

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

#include <loader.hpp>

#include <multiboot/multiboot_module_analyzer.hpp>
#include <memory/gdt/gdt_manager.hpp>
#include <memory/memory.hpp>
#include <memory/paging_initializer.hpp>
#include <memory/multiboot_mmap_interpreter.hpp>
#include <memory/bitmap/bitmap_page_allocator.hpp>
#include <kernelloader/kernel_loader.hpp>

#include <logger/logger.hpp>
#include <video/pretty_boot.hpp>
#include <video/console_video.hpp>
#include <multiboot/multiboot_util.hpp>
#include <stdarg.h>

/**
 * Linker symbols defined in the linker script
 */
extern "C" 
{
	void *endAddress;
}

/**
 * Setup information struct containing important information that the
 * kernel needs for further setup.
 */
static SetupInformation setupInformation;

/**
 * Returns the loaders area struct
 */
SetupInformation *EvaLoader::getSetupInformation() 
{
	return &setupInformation;
}

/**
 *
 */
static BitMapPageAllocator physicalAllocator;

/**
 *
 */
BitMapPageAllocator *EvaLoader::getPhysicalAllocator() 
{
	return &physicalAllocator;
}

/**
 * 
 */
uint32_t EvaLoader::findFreeMemory(MultibootInformation *info, uint32_t start, int count) 
{

	logInfo("%! searching for %i free pages (starting at %h)", "Eva Loader", count, start);
	PhysicalAddress location = start;

	while (location < 0xFFFFFFFF) 
	{

		bool notWithinModule = true;

		// For each of the required pages, check if it is within a module
		for (int i = 0; i < count; i++) 
		{
			uint32_t pos = location + i * PAGE_SIZE;

			// Check one of the modules contains this position
			for (uint32_t i = 0; i < info->modulesCount; i++) 
			{
				MultibootModule *module = (MultibootModule*) (info->modulesAddress + sizeof(MultibootModule) * i);

				uint32_t moduleStart = PAGE_ALIGN_DOWN(module->moduleStart);
				uint32_t moduleEnd = PAGE_ALIGN_UP(module->moduleEnd);

				if (pos >= moduleStart && pos < moduleEnd) 
				{
					notWithinModule = false;
					location = moduleEnd;
					break;
				}
			}
		}

		if (notWithinModule) 
		{
			logInfo("%# found: %h", location);
			return location;
		}

		location = location + PAGE_SIZE;
	}

	panic("%! could not find free memory chunk", "Eva Loader");
	return 0;
}

/**
 * 
 */
void EvaLoader::initialize(MultibootInformation *multibootInformation) 
{

	// Store multiboot structure
	setupInformation.multibootInformation = multibootInformation;

	// Begin initialization
	logInfo("%! loader initializing", "Eva Loader");

	// End of the loader binary in memory
	uint32_t loaderEndAddress = PAGE_ALIGN_UP((uint32_t ) &endAddress);

	// Find free spaces to place the GDT and the bitmap
	PRETTY_BOOT_STATUS("Initiliazing memory", 3, RED);
	uint32_t gdtAreaStart = findFreeMemory(multibootInformation, loaderEndAddress, 1);
	uint32_t gdtAreaEnd = gdtAreaStart + PAGE_SIZE;

	uint32_t bitmapStart = findFreeMemory(multibootInformation, gdtAreaEnd, PAGE_ALIGN_UP(BITMAP_SIZE) / PAGE_SIZE);
	uint32_t bitmapEnd = PAGE_ALIGN_UP(bitmapStart + BITMAP_SIZE);

	// The "reservedAreaEnd" is the end of the memory (somewhere above 1MiB)
	// that is not occupied by the loader binary or the pages that we split
	// of for use as bitmap and GDT.
	uint32_t reservedAreaEnd = bitmapEnd;

#if LOGGING_DEBUG
	// Information output
	logDebug("%! available modules:", "mmodule");
	for (uint32_t i = 0; i < multibootInformation->modulesCount; i++) 
	{
		MultibootModule* module = (MultibootModule*) (multibootInformation->modulesAddress + sizeof(MultibootModule) * i);
		logDebug("%#   '%s' at %h - %h", module->path, module->moduleStart, module->moduleEnd);
	}

	logDebug("%! calculated addresses:", "Eva Loader");
	logDebug("%#   gdt area:            %h", gdtAreaStart);
	logDebug("%#   bitmap:              %h", bitmapStart);
	logDebug("%#   reserved area end:   %h", reservedAreaEnd);
#endif

	// Store setup information
	setupInformation.bitmapStart = bitmapStart;
	setupInformation.bitmapEnd = bitmapEnd;

	// Set up the GDT. Here we pass the address of the gdt area, which contains enough space to
	// create the descriptor table and its pointer.
	GdtManager::initialize(gdtAreaStart);

	// Read GRUB map to add free pages to the allocator
	physicalAllocator.initialize((BitMapEntry*) bitmapStart);
	MultibootMmapInterpreter::load(&physicalAllocator, reservedAreaEnd);

	// Set up paging, this relocates the multiboot modules
	PagingInitializer::initialize(reservedAreaEnd, &setupInformation);
	// IMPORTANT: Now the multiboot module location has changed!

	// Load kernel binary
	PRETTY_BOOT_STATUS("locating Evangelion kernel", 3, GREEN);
	logInfo("%! locating Evangelion core...", "Eva Loader");
	MultibootModule *kernelModule = MultibootUtils::findModule(setupInformation.multibootInformation, "/boot/Evangelion_K/Eva_core/EvangelionNG");
	if (kernelModule) 
	{
		PRETTY_BOOT_STATUS("Loading EvangelionNG", 5, GREEN);
		logInfo("%! found EvangelionNG binary at %h, loading...", "Eva Loader", kernelModule->moduleStart);
		
		KernelLoader::load(kernelModule);
		EvaLoader::panic("%! something went wrong during boot process, halting", "Eva Loader");
	} 

	else 
	{
		PRETTY_BOOT_FAIL("EvangelionNG not found");
		EvaLoader::panic("%! EvangelionNG not found", "Eva Loader");
	}
}

/**
 * 
 */
void EvaLoader::panic(const char *msg, ...) 
{
	logInfo("%! an unrecoverable error has occured. reason:", "EvaLoader Panic");

	va_list valist;
	va_start(valist, msg);
	Logger::printFormatted(msg, valist);
	va_end(valist);
	Logger::printCharacter('\n');

	asm("cli");
	for (;;) 
	{
		asm("hlt");
	}
}

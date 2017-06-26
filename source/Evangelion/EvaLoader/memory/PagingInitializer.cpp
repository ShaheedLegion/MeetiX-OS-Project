/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <memory/PagingInitializer.hpp>
#include <memory/paging.hpp>
#include <memory/bitmap/BitmapPageAllocator.hpp>

#include <loader.hpp>
#include <logger/logger.hpp>

/**
 *
 */
void PagingInitializer::initialize(uint32_t reservedAreaEnd, SetupInformation *setupInformation)
{
	uint32_t pageDirectoryAddress = EvaLoader::getPhysicalAllocator()->allocate();
	setupInformation->initialPageDirectoryPhysical = pageDirectoryAddress;
	uint32_t* pageDirectory = (uint32_t*) pageDirectoryAddress;

	// Clear page directory
	for (uint32_t i = 0; i < 1024; i++)
		pageDirectory[i] = 0;

	// Map last entry to self
	pageDirectory[1023] = pageDirectoryAddress | DEFAULT_KERNEL_TABLE_FLAGS;

	// Identity-map reserved area;
	// User code must be able to access the lower memory, for example to access
	// memory blocks allocated in lower memory for VM86
	identityMap(pageDirectory, 0, reservedAreaEnd,
	DEFAULT_USER_TABLE_FLAGS,
	DEFAULT_USER_PAGE_FLAGS);

	// Relocate multiboot modules by mapping them right behind "reservedAreaEnd"
	relocateMultibootModules(pageDirectory, reservedAreaEnd);

	// Enable "global" page flags
	enableGlobalPageFlag();

	// Switch to created directory and enable paging
	switchToSpace(pageDirectory);
	enable();
}

/**
 *
 */
void PagingInitializer::enableGlobalPageFlag()
{
	uint32_t cr4;
	asm("mov %%cr4, %0" : "=r"(cr4));
	cr4 |= (1 << 7); // set page global enable bit
	asm("mov %0, %%cr4" :: "b"(cr4));
}

/**
 *
 */
void PagingInitializer::relocateMultibootModules(PageDirectory pageDirectory, uint32_t reservedAreaEnd)
{
	MultibootInformation *mbInfo = EvaLoader::getSetupInformation()->multibootInformation;
	uint32_t nextModuleLocation = reservedAreaEnd;

	for (uint32_t i = 0; i < mbInfo->modulesCount; i++)
	{
		MultibootModule *module = (MultibootModule*) (mbInfo->modulesAddress + sizeof(MultibootModule) * i);

		// Get physical values for module
		uint32_t modPhysStartAligned = PAGE_ALIGN_DOWN(module->moduleStart);
		uint32_t modPhysOff = module->moduleStart - modPhysStartAligned;
		uint32_t modPhysEndAligned = PAGE_ALIGN_UP(module->moduleEnd);
		uint32_t modPhysLen = module->moduleEnd - module->moduleStart;

		// Calculate virtual values and map
		uint32_t modVirtStartAligned = nextModuleLocation;
		uint32_t modVirtStart = modVirtStartAligned + modPhysOff;

		for (uint32_t off = 0; off < modPhysEndAligned - modPhysStartAligned; off = off + PAGE_SIZE)
		{
			mapPage(pageDirectory, modVirtStartAligned + off, modPhysStartAligned + off,
			DEFAULT_USER_TABLE_FLAGS,
			DEFAULT_USER_PAGE_FLAGS);
		}

		// Finish relocation by updating multiboot structure
		logDebugn("%! relocated, mapped phys %h-%h", "mmodule", module->moduleStart, module->moduleEnd);
		module->moduleStart = modVirtStart;
		module->moduleEnd = modVirtStart + modPhysLen;
		logDebug(" to virt %h-%h", module->moduleStart, module->moduleEnd);

		nextModuleLocation = PAGE_ALIGN_UP(module->moduleEnd);
	}
}

/**
 *
 */
void PagingInitializer::identityMap(uint32_t* directory, uint32_t start, uint32_t end, uint32_t tableFlags, uint32_t pageFlags)
{
	logDebug("%! identity-mapping: %h - %h", "paging", start, end);

	while (start < end)
	{
		mapPage(directory, start, start, tableFlags, pageFlags);
		start = start + PAGE_SIZE;
	}
}

/**
 *
 */
void PagingInitializer::mapPage(uint32_t* directory, uint32_t virtualAddress, uint32_t physicalAddress, uint32_t tableFlags, uint32_t pageFlags)
{
	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(virtualAddress);
	uint32_t pi = PAGE_IN_TABLE_INDEX(virtualAddress);

	// Create table if it does not exist
	if (!directory[ti])
	{
		uint32_t tableChunkAddress = (uint32_t) EvaLoader::getPhysicalAllocator()->allocate();

		uint32_t* table = (uint32_t*) tableChunkAddress;
		for (uint32_t i = 0; i < 1024; i++)
			table[i] = 0;

		directory[ti] = tableChunkAddress | tableFlags;
	}

	// Insert address into table
	uint32_t* table = (uint32_t*) (directory[ti] & 0xFFFFF000);
	if (!table[pi])
		table[pi] = physicalAddress | pageFlags;

	else
	{
		logDebug("%! tried to map area that was already mapped, virt %h -> phys %h, table contains %h", "paging", virtualAddress, physicalAddress, table[pi]);
		EvaLoader::panic("VirtualMemoryManager::addMapping - Duplicate mapping!");
	}

	// Flush address
	INVLPG(virtualAddress);
}

/**
 *
 */
bool PagingInitializer::mapPageToRecursiveDirectory(uint32_t virtualAddress, uint32_t physicalAddress, uint32_t tableFlags, uint32_t pageFlags)
{
	if ((virtualAddress & PAGE_ALIGN_MASK) || (physicalAddress & PAGE_ALIGN_MASK)) EvaLoader::panic("%! tried to map unaligned addresses: virt %h to phys %h", "paging", virtualAddress, physicalAddress);

	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(virtualAddress);
	uint32_t pi = PAGE_IN_TABLE_INDEX(virtualAddress);

	uint32_t *directory = (uint32_t*) 0xFFFFF000;
	uint32_t *table = ((uint32_t*) 0xFFC00000) + (0x400 * ti);

	// Create table if it does not exist
	if (!directory[ti])
	{
		uint32_t tableChunkAddress = (uint32_t) EvaLoader::getPhysicalAllocator()->allocate();
		directory[ti] = tableChunkAddress | tableFlags;

		// Zero the new table
		for (uint32_t i = 0; i < 1024; i++)
			table[i] = 0;
	}

	// Insert address into table
	if (!table[pi])
	{
		table[pi] = physicalAddress | pageFlags;

		// Flush address
		INVLPG(virtualAddress);

		return true;
	}

	else logDebug("%! tried to map area that was already mapped, virt %h -> phys %h, table contains %h", "paging", virtualAddress, physicalAddress, table[pi]);
	return false;
}

/**
 *
 */
void PagingInitializer::switchToSpace(PageDirectory directory)
{
	asm volatile("mov %0, %%cr3":: "b"(directory));
}

/**
 *
 */
void PagingInitializer::enable()
{
	logDebug("%! enabled", "paging");

	uint32_t cr0;
	asm volatile("mov %%cr0, %0": "=b"(cr0));
	cr0 |= 0x80000000;
	asm volatile("mov %0, %%cr0":: "b"(cr0));
}

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

#include <memory/gdt/gdt_manager.hpp>
#include <memory/gdt/gdt_mounter.hpp>

#include <logger/logger.hpp>

/**
 * 
 */
void GdtManager::initialize(uint32_t usableAddress) 
{

	uint32_t gdtPointerAddress = usableAddress;
	uint32_t gdtAddress = gdtPointerAddress + sizeof(GdtPointer);

	// Create the GDT pointer
	GdtPointer *gdtPointer = (GdtPointer*) gdtPointerAddress;
	gdtPointer->limit = (sizeof(GdtEntry) * 3) - 1;
	gdtPointer->base = gdtAddress;

	// Create the GDT
	GdtEntry *gdt = (GdtEntry*) gdtAddress;

	// Null descriptor, position 0x00
	Gdt::createGate(&gdt[0], 0, 0, 0, 0);

	// Kernel code segment descriptor, position 0x08
	Gdt::createGate(&gdt[1], 0, 0xFFFFFFFF, ACCESS_BYTE__KERNEL_CODE_SEGMENT, 0xCF);

	// Kernel data segment descriptor, position 0x10
	Gdt::createGate(&gdt[2], 0, 0xFFFFFFFF, ACCESS_BYTE__KERNEL_DATA_SEGMENT, 0xCF);

	// Load the GDT
	logDebug("%! descriptor table created at %h", "initgdt", gdtAddress);
	logDebug("%! pointer at %h with base %h and limit %h", "initgdt", gdtPointer, gdtPointer->base, gdtPointer->limit);
	_loadGdt(usableAddress);
	logDebug("%! initialized", "initgdt");
}

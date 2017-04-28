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

#ifndef SYSTEM_ACPI_MADT
#define SYSTEM_ACPI_MADT

#include <system/acpi/acpi_table_header.hpp>
#include "eva/stdint.h"

/**
 *
 */
struct MadtHeader 
{
	AcpiTableHeader header;

	uint32_t localControllerAddress;
	uint32_t flags;
}__attribute__((packed));

/**
 * Header that each entry has
 */
struct MadtEntryHeader 
{
	uint8_t deviceType;
	uint8_t recordLength;
}__attribute__((packed));

/**
 * Entry describing a local APIC
 */
struct MadtLapicEntry 
{
	MadtEntryHeader header;
	uint8_t processorId;
	uint8_t apicId;
	uint32_t flags;
}__attribute__((packed));

/**
 * Entry describing an IO APIC
 */
struct MadtIoapicEntry 
{
	MadtEntryHeader header;

	uint8_t ioapicId;
	uint8_t reserved0;
	uint32_t ioapicAddress;
	uint32_t globalSystemInterruptBase;
}__attribute__((packed));

/**
 * Entry describing an interrupt source override
 */
struct MadtInterruptSrcOverrideEntry 
{
	MadtEntryHeader header;

	uint8_t busSource;
	uint8_t irqSource;
	uint32_t globalSystemInterrupt;
	uint16_t flags;
}__attribute__((packed));

/**
 * Reads the Multiple APIC Descriptor Table
 */
class Madt 
{
public:
	static void parse(AcpiTableHeader* madtHeader);
};

#endif

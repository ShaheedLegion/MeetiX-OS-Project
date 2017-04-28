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

#ifndef SYSTEM_ACPI_ACPI
#define SYSTEM_ACPI_ACPI

#include "eva/stdint.h"
#include "eva/kernel.h"
#include "eva/types.h"
#include <system/acpi/acpi_table_header.hpp>
#include <system/acpi/acpi_entry.hpp>
#include <system/acpi/rsdp.hpp>

// Set a maximum for SDTs to avoid problems with possible junk tables
#define	G_SDT_MAXIMUM_BYTES		0x10000

/**
 *
 */
class Acpi 
{
private:

	/**
	 * Prepares the root SDT (RSTD or XSDT) by mapping the required
	 * memory into the virtual address space.
	 */
	static void prepareRootSDT(RsdpDescriptor *rsdp);

	/**
	 * Temporarily maps the SDT at the given location to the virtual address space,
	 * retrieves its length and then unmaps it again.
	 *
	 * @return the length of the table or 0 if failed
	 */
	static uint32_t getLengthOfUnmappedSDT(PhysicalAddress tableLocation);

	/**
	 * Maps the table at the given physical address into the virtual space.
	 * First uses the {getLengthOfUnmappedSDT} to get the length of the table
	 * and then maps the required size in the address space.
	 *
	 * @return header of the mapped table or 0
	 */
	static AcpiTableHeader *mapSDT(PhysicalAddress tableLocation);

	/**
	 * Validates the whole system descriptor table with the given header
	 */
	static bool validateSDT(AcpiTableHeader *header);

	/**
	 * Returns the number of entries that the RSDT with the given header has
	 */
	static uint32_t getRSDTentryCount();

	/**
	 * Returns the RSDT entry at the given index.
	 */
	static PhysicalAddress getRSDTentry(uint32_t index);

public:

	/**
	 *
	 */
	static AcpiEntry *getEntryWithSignature(const char *signature);

	/**
	 *
	 */
	static AcpiEntry *getEntries();

	/**
	 *
	 */
	static bool hasEntries();

	/**
	 *
	 */
	static void gatherInformation();

};

#endif

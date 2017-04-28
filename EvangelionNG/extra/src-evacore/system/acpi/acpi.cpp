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

#include <system/acpi/acpi.hpp>
#include <system/acpi/rsdp_lookup_util.hpp>
#include <logger/logger.hpp>
#include <kernel.hpp>
#include <utils/string.hpp>
#include <memory/address_space.hpp>
#include <system/acpi/madt.hpp>
#include "eva/types.h"

/**
 * Remembers the root header and whether its an XSDT
 */
static AcpiTableHeader *rootHeader = 0;
static bool rootIsXSDT = false;

/**
 * List of all tables in the root
 */
static AcpiEntry *first = 0;

/**
 *
 */
AcpiEntry *Acpi::getEntries() 
{
	return first;
}

/**
 * 
 */
bool Acpi::hasEntries() 
{
	return first != 0;
}

/**
 *
 */
AcpiEntry *Acpi::getEntryWithSignature(const char *signature) 
{
	AcpiEntry *cur = first;
	while (cur) 
	{
		if (cur->hasSignature(signature)) 
		{
			return cur;
		}
		cur = cur->next;
	}
	return 0;
}

/**
 *
 */
void Acpi::gatherInformation() 
{

	logDebug("%! gathering information", "acpi");

	// Find root table pointer
	RsdpDescriptor *rsdp = RsdpLookupUtil::findRSDP();
	if (rsdp) 
	{

		// Prepare the root table
		Acpi::prepareRootSDT(rsdp);

		// Go through the existing root entries
		for (uint32_t i = 0; i < getRSDTentryCount(); i++) 
		{
			PhysicalAddress entry = getRSDTentry(i);
			if (entry != 0) 
			{
				AcpiTableHeader *sdt = mapSDT(entry);

				// Could not be mapped? Skip
				if (sdt == 0) 
				{
					continue;
				}

				// Create the entry
				AcpiEntry *entry = new AcpiEntry();
				entry->header = sdt;
				entry->next = first;
				first = entry;
			}
		}

	} 

	else 
	{
		logDebug("%! could not find RSDP", "acpi");
	}
}

/**
 * 
 */
void Acpi::prepareRootSDT(RsdpDescriptor *rsdp) 
{

	PhysicalAddress rootTableLocation = 0;

	rootIsXSDT = false;

	// If ACPI 2.0 or higher, try to use the XSDT
	if (rsdp->revision > 0) 
	{

		RsdpDescriptor20* rsdp20 = (RsdpDescriptor20*) rsdp;
		if (rsdp20->xsdtAddress != 0) 
		{
#if _ARCH_X86_64_
			rootTableLocation = rsdp20->xsdtAddress;
			logDebug("%! found XSDT in 64bit range", "acpi");
			rootIsXSDT = true;
#elif _ARCH_X86_
			if (rsdp20->xsdtAddress < 0xFFFFFFFF) 
			{
				rootIsXSDT = true;
				logDebug("%! found XSDT in 32bit range", "acpi");
				rootTableLocation = rsdp20->xsdtAddress;
			} 

			else 
			{
				rootIsXSDT = false;
				logDebug("%! found XSDT, but range too high for 32bits, attempting to use RSDT", "acpi");
			}
#endif
		}
	}

	// No XSDT? Use RSDT
	if (!rootIsXSDT) 
	{
		logDebug("%! no XSDT; using RSDT", "acpi");
		rootTableLocation = rsdp->rsdtAddress;
	}

	// No header? Failed
	if (rootTableLocation == 0) 
	{
		logWarn("%! RSDP did not contain a valid RSDT/XSDT address", "acpi");
		return;
	}

	logDebug("%! root table starts at phys %h", "acpi", rootTableLocation);

	// Map table in virtual address space
	AcpiTableHeader *header = mapSDT(rootTableLocation);
	if (header == 0) 
	{
		logWarn("%! could not map root system descriptor table", "acpi");
		return;
	}

	rootHeader = header;
}

/**
 * 
 */
bool Acpi::validateSDT(AcpiTableHeader *header) 
{

	uint8_t sum = 0;
	uint8_t* tableBytes = reinterpret_cast<uint8_t*>(header);

	for (uint32_t i = 0; i < header->length; i++) 
	{
		sum += tableBytes[i];
	}

	return sum == 0;
}

/**
 * 
 */
uint32_t Acpi::getLengthOfUnmappedSDT(PhysicalAddress tableLocation) 
{

	// Align the location down, we will allocate 2 pages to make sure the
	// header is within the range
	PhysicalAddress physStart = PAGE_ALIGN_DOWN(tableLocation);
	VirtualAddress virtualBase = EvaKernel::evaKernelRangePool->allocate(2);

	if (!AddressSpace::map(virtualBase, physStart, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS)) 
	{
		logWarn("%! could not create virtual mapping (1) for SDT %h", "acpi", tableLocation);
		return 0;
	}
	if (!AddressSpace::map(virtualBase + PAGE_SIZE, physStart + PAGE_SIZE, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS)) 
	{
		logWarn("%! could not create virtual mapping (2) for SDT %h", "acpi", tableLocation);
		return 0;
	}

	// Calculate the offset of the header within the table
	uint32_t mappingOffset = tableLocation - physStart;

	// Take length from the header
	AcpiTableHeader *header = (AcpiTableHeader*) (virtualBase + mappingOffset);
	uint32_t length = header->length;

	// Unmap the two mapped pages
	AddressSpace::unmap(virtualBase);
	AddressSpace::unmap(virtualBase + PAGE_SIZE);
	EvaKernel::evaKernelRangePool->free(virtualBase);

	return length;
}

/**
 * 
 */
AcpiTableHeader *Acpi::mapSDT(PhysicalAddress tableLocation) 
{

	// Retrieve the tables length
	uint32_t tableLength = getLengthOfUnmappedSDT(tableLocation);
	if (tableLength == 0) 
	{
		logWarn("%! could not map SDT at phys %h, could not get table length", "acpi", tableLocation);
		return 0;
	}

	// Does the length make sense?
	if (tableLength > G_SDT_MAXIMUM_BYTES) 
	{
		logWarn("%! SDT at %h was skipped due to illegal length (%h)", "acpi", tableLocation, tableLength);
		return 0;
	}

	// Down/upalign physical range
	PhysicalAddress physStart = PAGE_ALIGN_DOWN(tableLocation);
	PhysicalAddress physEnd = PAGE_ALIGN_UP(tableLocation + tableLength);

	// Calculate offset of header within first page
	uint32_t mappingOffset = tableLocation - physStart;

	// Calculate amount of physical pages and allocate virtual range
	uint32_t pages = (physEnd - physStart) / PAGE_SIZE;
	VirtualAddress virtualBase = EvaKernel::evaKernelRangePool->allocate(pages);

	// Could not find a virtual range of that size
	if (virtualBase == 0) 
	{
		logWarn("%! could not find a free virtual range to map an SDT of size %i pages", "acpi", pages);
		return 0;
	}

	// Map the pages
	for (VirtualAddress off = 0; off < (physEnd - physStart); off = off + PAGE_SIZE) 
	{
		AddressSpace::map(virtualBase + off, physStart + off, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);
	}

	// Get the header pointer
	AcpiTableHeader *header = (AcpiTableHeader*) (virtualBase + mappingOffset);

	// Validate the table
	if (!validateSDT(header)) 
	{
		logWarn("%! descriptor table was not valid", "acpi");
		return 0;
	}

	// Now return the tables header
	return header;
}

/**
 * 
 */
uint32_t Acpi::getRSDTentryCount() 
{

	uint32_t entryBytes = rootHeader->length - sizeof(AcpiTableHeader);
	if (rootIsXSDT) 
	{
		return entryBytes / 8;
	} 

	else 
	{
		return entryBytes / 4;
	}
}

/**
 * 
 */
PhysicalAddress Acpi::getRSDTentry(uint32_t index) 
{

	VirtualAddress startOfEntries = ((VirtualAddress) rootHeader) + sizeof(AcpiTableHeader);

	if (rootIsXSDT) 
	{
		return ((uint64_t*) startOfEntries)[index];
	} 

	else 
	{
		return ((uint32_t*) startOfEntries)[index];
	}

	return 0;
}

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

#include "pci.hpp"
#include "logger/logger.hpp"
#include "system/IOPorts.hpp"
#include "utils/ListEntry.hpp"

static ListEntry<PciHeader*> *pciHeaders = 0;
static uint32_t pciHeadersFound = 0;

/**
 *
 */
void Pci::initialize()
{
	scanBusesByBruteforce();
	logInfo("%! found %i devices", "pci", pciHeadersFound);
}

/**
 *
 */
PciHeader *Pci::findDeviceAt(uint8_t bus, uint8_t slot, uint8_t function)
{
	ListEntry<PciHeader*> *next = pciHeaders;
	while (next)
	{
		PciHeader *header = next->value;
		if (header->bus == bus && header->slot == slot && header->function == function) return header;
		next = next->next;
	}

	return 0;
}

/**
 * Searches for a device of the requested type.
 */
PciHeader *Pci::findDeviceOfType(uint8_t classCode, uint8_t subclassCode, int index)
{
	int pos = 0;
	ListEntry<PciHeader*> *next = pciHeaders;
	while (next)
	{
		PciHeader *header = next->value;
		if (classCode == header->classCode && subclassCode == header->subclassCode)
		{
			if (pos == index) return header;
			pos++;
		}
		next = next->next;
	}

	return 0;
}

/**
 *
 */
void Pci::scanBusesByBruteforce()
{
	logInfo("%! scanning bus with brute force", "pci");

	for (uint32_t bus = 0; bus < 256; bus++)
	{
		for (uint32_t slot = 0; slot < 32; slot++)
		{
			// check for a valid device in function 0
			if (isValidDevice(bus, slot, 0))
			{
				// check if it's a multifunction device
				if (isMultifunctionDevice(bus, slot))
				{
					// device may have up to 8 functions, check each
					for (int function = 0; function < 8; function++)
					{
						// check if function is valid
						if (isValidDevice(bus, slot, function)) createDeviceHeader(bus, slot, function);
					}
				}

				else createDeviceHeader(bus, slot, 0);
			}
		}
	}
}

/**
 *
 */
void Pci::createDeviceHeader(uint8_t bus, uint8_t slot, uint8_t function)
{
	// read header type
	PciHeader *header = createHeader();
	header->bus = bus;
	header->slot = slot;
	header->function = function;

	uint8_t headerTypeWithMultifuncBit = (readConfiguration(bus, slot, function, 0x0C) & 0xFF0000) >> 16;
	header->headerType = headerTypeWithMultifuncBit & 0x7F;
	header->multiFunction = (headerTypeWithMultifuncBit & 0x80) >> 7;

	// read information common to all headers
	uint32_t entry00 = readConfiguration(bus, slot, function, 0x00);
	header->deviceId = entry00 >> 16;
	header->vendorId = entry00 & 0xFFFF;

	uint32_t entry08 = readConfiguration(bus, slot, function, 0x08);
	header->classCode = (entry08 & 0xFF000000) >> 24;
	header->subclassCode = (entry08 & 0xFF0000) >> 16;
	header->progIf = (entry08 & 0xFF00) >> 8;
	header->revisionId = (entry08 & 0xFF);
}

/**
 *
 */
uint32_t Pci::readConfiguration(uint8_t bus, uint8_t slot, uint8_t function, uint8_t reg)
{
	uint32_t address = (((uint32_t) bus << 16) | ((uint32_t) slot << 11) | ((uint32_t) function << 8) | (reg & 0xFC) | ENABLE_BIT);

	// write address to address field
	IOports::writeInt(CONFIG_ADDRESS, address);

	// read data from data field
	return IOports::readInt(CONFIG_DATA);
}

/**
 *
 */
bool Pci::isValidDevice(uint8_t bus, uint8_t slot, uint8_t function)
{
	uint16_t vendor = readConfiguration(bus, slot, function, 0) & 0xFFFF;

	// if invalid vendor, this is not a valid device
	if (vendor == 0xFFFF) return false;

	return true;
}

/**
 *
 */
bool Pci::isMultifunctionDevice(uint8_t bus, uint8_t slot)
{
	uint8_t headerType = (readConfiguration(bus, slot, 0, 0x0C) & 0xFF0000) >> 16;
	return (headerType & 0x80);
}

/**
 *
 */
PciHeader *Pci::createHeader()
{
	PciHeader *header = new PciHeader();

	ListEntry<PciHeader*> *entry = new ListEntry<PciHeader*>();
	entry->next = pciHeaders;
	entry->value = header;
	pciHeaders = entry;

	pciHeadersFound++;

	return header;
}

/**
 *
 */
uint32_t Pci::getDeviceCount()
{
	return pciHeadersFound;
}

/**
 *
 */
PciHeader *Pci::getDeviceAt(uint32_t position)
{
	uint32_t pos = 0;
	ListEntry<PciHeader*>* next = pciHeaders;
	while (next)
	{
		if (position == pos) return next->value;
		++pos;
		next = next->next;
	}

	return nullptr;
}

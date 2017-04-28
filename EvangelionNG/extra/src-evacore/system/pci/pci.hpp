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

#ifndef __EVA_SYSTEM_PCI_PCI__
#define __EVA_SYSTEM_PCI_PCI__

#include <eva/stdint.h>

#define ENABLE_BIT		((uint32_t) 0x80000000)
#define CONFIG_ADDRESS	0xCF8
#define CONFIG_DATA		0xCFC

/**
 *
 */
class PciHeader 
{
public:
	uint8_t bus;
	uint8_t slot;
	uint8_t function;

	uint16_t vendorId;
	uint16_t deviceId;

	uint8_t classCode;
	uint8_t subclassCode;
	uint8_t progIf;
	uint8_t revisionId;

	uint8_t headerType;
	bool multiFunction;
	uint8_t numberOfFunctions;
};

/**
 * Class codes
 */
#define PCI_CLASS_VARIOUS					0x00
#define PCI_CLASS_MASS_STORAGE				0x01
#define PCI_CLASS_NETWORK					0x02
#define PCI_CLASS_DISPLAY					0x03
#define PCI_CLASS_MULTIMEDIA				0x04
#define PCI_CLASS_MEMORY					0x05
#define PCI_CLASS_BRIDGE					0x06
#define PCI_CLASS_SIMPLE_COM				0x07
#define PCI_CLASS_BASE_SYS_PERIPHERALS		0x08
#define PCI_CLASS_INPUT_DEVICES				0x09
#define PCI_CLASS_DOCKING_STATIONS			0x0A
#define PCI_CLASS_PROCESSORS				0x0B
#define PCI_CLASS_SERIAL_BUS_CONT			0x0C
#define PCI_CLASS_WIRELESS_CONT				0x0D
#define PCI_CLASS_INTELLI_IO_CONT			0x0E
#define PCI_CLASS_SATELLITE_COM_CONT		0x0F
#define PCI_CLASS_ENCDEC_CONT				0x10
#define PCI_CLASS_DATAACQ_SIGPROC_CONT		0x11
#define PCI_CLASS_UNKNOWN					0xFF

/**
 *
 */
class Pci 
{
public:
	static void initialize();

	static uint32_t readConfiguration(uint8_t bus, uint8_t slot, uint8_t function, uint8_t reg);

	/**
	 * Simple brute-force scanning implementation that checks each slot on each bus if there
	 * is a valid device available.
	 */
	static void scanBusesByBruteforce();

	/**
	 * Creates the device header that we remember in our global device list.
	 */
	static void createDeviceHeader(uint8_t bus, uint8_t slot, uint8_t function);

	/**
	 * Searches for a device of the requested type.
	 */
	static PciHeader *findDeviceOfType(uint8_t classCode, uint8_t subclassCode, int index);
	static PciHeader *findDeviceAt(uint8_t bus, uint8_t slot, uint8_t function);

	static uint32_t getDeviceCount();
	static PciHeader *getDeviceAt(uint32_t position);

	/**
	 * Various check functions
	 */
	static bool isValidDevice(uint8_t bus, uint8_t slot, uint8_t function);
	static bool isMultifunctionDevice(uint8_t bus, uint8_t slot);

private:
	static PciHeader *createHeader();
};

#endif

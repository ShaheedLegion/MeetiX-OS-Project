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

#include <system/acpi/RsdpLookupUtil.hpp>
#include <system/BiosDataArea.hpp>
#include <logger/logger.hpp>

/**
 *
 */
RsdpDescriptor *RsdpLookupUtil::findRSDPinRange(uint32_t start, uint32_t end)
{
	uint32_t pos = start;
	while (pos < end)
	{
		uint64_t val = *((uint64_t*) pos);
		if (val == RSDPTR_MAGIC) return (RsdpDescriptor*) pos;

		// RSDP is always at 16 byte boundary
		pos += 16;
	}

	return 0;
}

/**
 *
 */
RsdpDescriptor *RsdpLookupUtil::findRSDPinEBDA()
{
	uint32_t ebdaAddress = biosDataArea->ebdaShiftedAddr << 4;

	// If the EBDA address is above 1MiB, we can assume its not valid.
	// Also we want to scan the first KiB, so we check it like this:
	if (ebdaAddress + 0x400 >= 0x100000)
	{
		logWarn("%! address of EBDA was at %h (not valid)", "rsdplookup", ebdaAddress);
		return 0;
	}

	// Scan first kilobyte of the EBDA for RDSP
	return findRSDPinRange(ebdaAddress, ebdaAddress + 0x400);
}

/**
 *
 */
bool RsdpLookupUtil::validateRSDP(RsdpDescriptor *rsdp)
{
	uint8_t baseCheckResult = 0;
	uint8_t *rsdpBytes = reinterpret_cast<uint8_t*>(rsdp);
	for (uint16_t i = 0; i < sizeof(RsdpDescriptor); i++)
		baseCheckResult += rsdpBytes[i];

	// Not zero checksum? Illegal RSDP
	if (baseCheckResult != 0) return false;

	logDebug("%! base part of RSDP is valid", "rsdplookup");

	// If we ACPI 2.0 (= revision 1) or higher we need to also validate the
	// additional fields of the RSDP separately:
	if (rsdp->revision > 0)
	{
		uint8_t extendedCheckResult = 0;
		for (uint16_t i = sizeof(RsdpDescriptor); i < sizeof(RsdpDescriptor20); i++)
			extendedCheckResult += rsdpBytes[i];

		if (extendedCheckResult != 0) return false;

		logDebug("%! extended part of RSDP is valid", "rsdplookup");
	}

	// All valid
	return true;
}

/**
 *
 */
RsdpDescriptor *RsdpLookupUtil::findRSDP()
{
	RsdpDescriptor *rsdp = 0;

	// Look in EBDA, Look in the area just below 1 MiB
	rsdp = findRSDPinEBDA();
	if (rsdp == 0) rsdp = findRSDPinRange(0x000E0000, 0x000FFFFF);

	// Check RSDP
	if (rsdp && validateRSDP(rsdp))
	{
		logDebugn("%! found valid pointer: oemId: '", "rsdplookup");
		for (uint16_t i = 0; i < 6; i++)
			logDebugn("%c", rsdp->oemId[i]);

		logDebugn("', acpiVersion: ");
		if (rsdp->revision == 0) { logDebugn("1.0"); }
		else if (rsdp->revision == 1) { logDebugn("2.0"); }
		else if (rsdp->revision == 2) { logDebugn("3.0"); }
		else { logDebugn("? (revision %i)", rsdp->revision); }

		logDebug("");

		return rsdp;
	}

	return 0;
}

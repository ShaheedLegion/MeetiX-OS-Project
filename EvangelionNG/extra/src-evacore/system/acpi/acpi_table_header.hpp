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

#ifndef SYSTEM_ACPI_ACPISDTHEADER
#define SYSTEM_ACPI_ACPISDTHEADER

#include "eva/stdint.h"

/**
 *
 */
struct AcpiTableHeader 
{
	uint8_t signature[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	uint8_t oemId[6];
	uint8_t oemTableId[8];
	uint32_t oemRevision;
	uint32_t creatorId;
	uint32_t creatorRevision;
}__attribute__((packed));

#endif

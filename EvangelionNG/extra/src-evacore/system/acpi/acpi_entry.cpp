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

#include <system/acpi/acpi_entry.hpp>
#include <utils/string.hpp>

/**
 * 
 */
bool AcpiEntry::hasSignature(const char *signature) 
{

	// SDT headers always have a signature with length 4
	if (String::length(signature) != 4) 
	{
		return false;
	}

	// Check if the headers signature is equal to the wanted signature
	for (int i = 0; i < 4; i++) 
	{
		if (signature[i] != header->signature[i]) 
		{
			return false;
		}
	}

	return true;
}

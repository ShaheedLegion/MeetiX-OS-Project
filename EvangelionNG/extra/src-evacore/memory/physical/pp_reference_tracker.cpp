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

#include <memory/physical/pp_reference_tracker.hpp>
#include <memory/paging.hpp>
#include <logger/logger.hpp>

PPreferenceCountDirectory directory;

/**
 *
 */
void PPreferenceTracker::increment(PhysicalAddress address) 
{

	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(address);
	uint32_t pi = PAGE_IN_TABLE_INDEX(address);

	if (directory.tables[ti] == 0) 
	{
		directory.tables[ti] = new PPreferenceCountTable;

		for (uint32_t i = 0; i < 1024; i++) 
		{
			directory.tables[ti]->referenceCount[i] = 0;
		}
	}

	++(directory.tables[ti]->referenceCount[pi]);
}

/**
 *
 */
int16_t PPreferenceTracker::decrement(PhysicalAddress address) 
{

	uint32_t ti = TABLE_IN_DIRECTORY_INDEX(address);
	uint32_t pi = PAGE_IN_TABLE_INDEX(address);

	if (directory.tables[ti] == 0) 
	{
		return 0;
	}

	return --(directory.tables[ti]->referenceCount[pi]);
}

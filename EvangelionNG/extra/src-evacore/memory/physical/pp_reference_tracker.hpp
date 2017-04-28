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

#ifndef PHYSICALPAGESHARETRACKER_HPP_
#define PHYSICALPAGESHARETRACKER_HPP_

#include "eva/stdint.h"
#include "eva/kernel.h"
#include "eva/types.h"

/**
 *
 */
struct PPreferenceCountTable 
{
	int16_t referenceCount[1024];
};

/**
 *
 */
struct PPreferenceCountDirectory 
{
	PPreferenceCountTable *tables[1024];
};

/**
 * Keeps track of the number of processes that reference pages
 * either in their process image or heap area.
 */
class PPreferenceTracker 
{
public:

	/**
	 *
	 */
	static void increment(PhysicalAddress address);

	/**
	 *
	 */
	static int16_t decrement(PhysicalAddress address);

};

#endif

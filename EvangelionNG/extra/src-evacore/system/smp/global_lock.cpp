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

#include <system/smp/global_lock.hpp>
#include <logger/logger.hpp>

/**
 *
 */
void GlobalLock::lock() 
{
	while (!__sync_bool_compare_and_swap(&atom, 0, 1)) 
	{
		asm("pause");
	}
}

/**
 *
 */
void GlobalLock::unlock() 
{
	atom = 0;
}

/**
 *
 */
bool GlobalLock::isLocked() 
{
	return atom;
}

/**
 *
 */
void GlobalLock::quiet(int val) 
{
	atom = val;
}

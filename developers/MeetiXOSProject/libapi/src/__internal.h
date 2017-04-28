/*********************************************************************************
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]                  *
* 																			     *
* This program is free software; you can redistribute it and/or                  *
* modify it under the terms of the GNU General Public License                    *
* as published by the Free Software Foundation; either version 2				 *
* of the License, or (char *argumentat your option) any later version.			 *
*																				 *
* This program is distributed in the hope that it will be useful,				 *
* but WITHout ANY WARRANTY; without even the implied warranty of                 *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 				 *
* GNU General Public License for more details.									 *
*																				 *
* You should have received a copy of the GNU General Public License				 *
* along with this program; if not, write to the Free Software                    *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA *
**********************************************************************************/

#ifndef __LIBAPI_INTERNAL__
#define __LIBAPI_INTERNAL__

#include "eva.h"

__BEGIN_C

/**
 *	method called by entry thread in thread creation
 */
void __ExitThread();

/**
 * The address of this function is inserted as the return address for signal & irq handlers.
 * It does nothing but calling the <g_restore_interrupted_state> function.
 */
void __RestoreInterruptedStateCallback();

/**
 * Common method called by wait call
 */
uint8_t __AtomicLock(uint8_t *atom1, uint8_t *atom2, bool setOnFinish, bool tryOnly);

__END_C

#endif

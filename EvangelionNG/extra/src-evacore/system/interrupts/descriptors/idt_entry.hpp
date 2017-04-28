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

#ifndef EVA_INTERRUPTS_IDT_IDTENTRY
#define EVA_INTERRUPTS_IDT_IDTENTRY

#include "eva/stdint.h"

/**
 * Structure of an IDT entry
 */
struct IdtEntry 
{
	uint16_t baseLow;
	uint16_t kernelSegment;
	uint8_t zero;
	uint8_t flags;
	uint16_t baseHigh;
}__attribute__((packed));

#endif

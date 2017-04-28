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

#ifndef __PS2_DRIVER_IRQ_TRIGGERED__
#define __PS2_DRIVER_IRQ_TRIGGERED__

/**
 * Is registered as an IRQ handler in the kernel. Handles the incoming requests,
 * reads data from the PS2 ports and dispatches the data.
 *
 * @param irq
 * 		irq to handle
 */
extern "C" void irqHandler(uint8_t irq);

#endif

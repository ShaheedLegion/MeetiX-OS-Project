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

#ifndef EVA_INTERRUPTS_IOAPICMANAGER
#define EVA_INTERRUPTS_IOAPICMANAGER

#include "eva/kernel.h"
#include "eva/stdint.h"
#include <system/interrupts/ioapic.hpp>

/**
 * I/O APIC manager
 */
class IoApicManager 
{
private:
	static IoApic *getResponsibleFor(uint32_t source);

public:
	static IoApic *getEntries();

	static void create(uint32_t id, PhysicalAddress ioapicAddress, uint32_t globalSystemInterruptBase);
	static bool areAvailable();

	static bool createIsaRedirectionEntry(uint32_t source, uint32_t irq, uint32_t destinationApic);
	static void maskIrq(uint32_t irq);
	static void unmaskIrq(uint32_t irq);

};

#endif

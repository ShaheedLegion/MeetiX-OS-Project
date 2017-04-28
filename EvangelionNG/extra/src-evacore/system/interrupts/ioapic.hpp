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

#ifndef EVA_INTERRUPTS_IOAPIC
#define EVA_INTERRUPTS_IOAPIC

#include "eva/stdint.h"
#include "eva/kernel.h"
#include "eva/types.h"

/**
 * The two memory-mapped offsets of the IOAPIC
 */
#define IOAPIC_REGSEL		0x00
#define IOAPIC_REGWIN		0x10

/**
 * Selectable registers
 */
#define IOAPIC_ID			0x00
#define IOAPIC_VER			0x01
#define IOAPIC_ARB			0x02
#define IOAPIC_REDTBL_BASE	0x10

/**
 * Values for REDTBL entries
 */
#define IOAPIC_REDTBL_INTVEC_MAKE(i)		((i) & 0xFF)

#define IOAPIC_REDTBL_DELMOD_FIXED			(0x0 << 8)	// 000
#define IOAPIC_REDTBL_DELMOD_LOWEST			(0x1 << 8)	// 001
#define IOAPIC_REDTBL_DELMOD_SMI			(0x2 << 8)	// 010
#define IOAPIC_REDTBL_DELMOD_NMI			(0x4 << 8)	// 100
#define IOAPIC_REDTBL_DELMOD_INIT			(0x5 << 8)	// 101
#define IOAPIC_REDTBL_DELMOD_EXTINT			(0x7 << 8)	// 111

#define IOAPIC_REDTBL_DESTMOD_PHYSICAL		(0 << 10)
#define IOAPIC_REDTBL_DESTMOD_LOGICAL		(1 << 10)

#define IOAPIC_REDTBL_DELIVS_IDLE			(0 << 11)
#define IOAPIC_REDTBL_DELIVS_SEND_PENDING	(1 << 11)

#define IOAPIC_REDTBL_INTPOL_HIGH_ACTIVE	(0 << 12)
#define IOAPIC_REDTBL_INTPOL_LOW_ACTIVE		(1 << 12)

#define IOAPIC_REDTBL_REMOTEIRR_REC_EOI		(0 << 13)
#define IOAPIC_REDTBL_REMOTEIRR_ACCEPTING	(1 << 13)

#define IOAPIC_REDTBL_TRIGGERMOD_EDGE		(0 << 14)
#define IOAPIC_REDTBL_TRIGGERMOD_LEVEL		(1 << 14)

#define IOAPIC_REDTBL_INTMASK_UNMASKED		(0 << 15)
#define IOAPIC_REDTBL_INTMASK_MASKED		(1 << 15)

#define IOAPIC_REDTBL_DESTINATION_MAKE(i, f)		((((uint64_t) i & 0xFF) | (uint64_t) f) << 56)
#define IOAPIC_REDTBL_DESTINATION_FLAG_PHYSICAL		(0 << 10)
#define IOAPIC_REDTBL_DESTINATION_FLAG_LOGICAL		(1 << 10)

/**
 * Masks for each entry
 */
#define IOAPIC_REDTBL_MASK_INTVEC			(0xFF)
#define IOAPIC_REDTBL_MASK_DELMOD			(7 << 8)
#define IOAPIC_REDTBL_MASK_DESTMOD			(1 << 10)
#define IOAPIC_REDTBL_MASK_DELIVS			(1 << 11)
#define IOAPIC_REDTBL_MASK_INTPOL			(1 << 12)
#define IOAPIC_REDTBL_MASK_REMOTEIRR		(1 << 13)
#define IOAPIC_REDTBL_MASK_TRIGGERMOD		(1 << 14)
#define IOAPIC_REDTBL_MASK_INTMASK			(1 << 15)
#define IOAPIC_REDTBL_MASK_RESERVED			(0xFFFFFFFFFFC << 16)
#define IOAPIC_REDTBL_MASK_DESTINATION		(0xFF << 55)

/**
 *
 */
class IoApic 
{
private:
	uint32_t id;

	PhysicalAddress physicalAddress;
	VirtualAddress virtualAddress;

	uint32_t globalSystemInterruptBase;
	uint32_t redirectEntryCount;

	// Stored by the manager in a singly linked list
	IoApic *next;

	void createMapping();

public:
	IoApic(uint32_t id, PhysicalAddress physicalAddress, uint32_t globalSystemInterruptBase, IoApic *next) : id(id), physicalAddress(physicalAddress), virtualAddress(0), globalSystemInterruptBase(globalSystemInterruptBase), redirectEntryCount(0), next(next) 
	{
	}

	/**
	 *
	 */
	void initialize();

	/**
	 *
	 */
	void write(uint32_t reg, uint32_t value);

	/**
	 *
	 */
	uint32_t read(uint32_t reg);

	/**
	 *
	 */
	uint64_t getRedirectionEntry(uint32_t index);

	/**
	 *
	 */
	void setRedirectionEntry(uint32_t index, uint64_t value);

	/**
	 *
	 */
	uint32_t getGlobalSystemInterruptBase() const 
	{
		return globalSystemInterruptBase;
	}

	/**
	 *
	 */
	uint32_t getRedirectEntryCount() const 
	{
		return redirectEntryCount;
	}

	/**
	 *
	 */
	IoApic *getNext() const 
	{
		return next;
	}

	/**
	 *
	 */
	void mask(uint32_t source);

	/**
	 *
	 */
	void unmask(uint32_t source);

};

#endif

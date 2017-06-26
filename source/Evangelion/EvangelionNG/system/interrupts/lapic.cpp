/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
*         DERIVED FROM THE GHOST OPERATING SYSTEM                                     *
*         This software is derived from the Ghost operating system project,           *
*         written by Max Schlüssel <lokoxe@gmail.com>. Copyright 2012-2017            *
*         https://ghostkernel.org/                                                    *
*         https://github.com/maxdev1/ghost                                            *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <system/interrupts/lapic.hpp>

#include <BuildConfig.hpp>
#include <system/IOPorts.hpp>
#include <logger/logger.hpp>
#include <memory/AddressSpace.hpp>
#include <system/timing/pit.hpp>
#include <EvangelionNG.hpp>
#include <system/processor.hpp>
#include <system/smp/GlobalLock.hpp>

static bool prepared = false;

// All APICs are at the same physical and virtual address
static PhysicalAddress physicalBase = 0;
static VirtualAddress virtualBase;

/**
 *
 */
void Lapic::initialize()
{
	// Read version
	#if LOGGING_DEBUG
	{
		uint32_t apicVersionRegVal = read(APIC_REGISTER_VERSION);

		uint32_t localId = readId();
		uint8_t apicVersion = apicVersionRegVal & 0xFF;
		uint16_t maxLvtIndex = (apicVersionRegVal >> 16) & 0xFF;
		logDebug("%! id %i, version %h (%s), maxlvtindex: %i", "lapic", localId, apicVersion, (apicVersion < 0x10 ? "82489DX discrete" : "integrated"), maxLvtIndex);
	}
	#endif

	// Initialize APIC to well-known state
	write(APIC_REGISTER_DEST_FORMAT, 0xFFFFFFFF);
	write(APIC_REGISTER_LOGICAL_DEST, (read(APIC_REGISTER_LOGICAL_DEST) & 0x00FFFFFF) | 1);
	write(APIC_REGISTER_LVT_TIMER, APIC_LVT_INT_MASKED);
	write(APIC_REGISTER_LVT_PERFMON, APIC_LVT_DELIVERY_MODE_NMI);
	write(APIC_REGISTER_LVT_LINT0, APIC_LVT_INT_MASKED);
	write(APIC_REGISTER_LVT_LINT1, APIC_LVT_INT_MASKED);
	write(APIC_REGISTER_TASK_PRIO, 0);

	// Enable the APIC
	write(APIC_REGISTER_SPURIOUS_IVT, 0xFF | APIC_SPURIOUS_IVT_SOFTWARE_ENABLE);

	// Set up timer
	startTimer();
}

/**
 *
 */
void Lapic::prepare(PhysicalAddress lapicAddress)
{
	physicalBase = lapicAddress;
	prepared = true;

	// Warn if APIC not at expected location
	if (physicalBase != EXPECTED_APIC_PHYSICAL_ADDRESS)
	{
		logInfo("%! is at %h, not %h as expected", "lapic", physicalBase, EXPECTED_APIC_PHYSICAL_ADDRESS);
	}
	logInfo("%! base is %h", "lapic", physicalBase);

	// Map it to virtual space
	createMapping();
}

/**
 *
 */
void Lapic::createMapping()
{
	// Map it to virtual space
	virtualBase = EvaKernel::evaKernelRangePool->allocate(1);
	if (virtualBase == 0) EvaKernel::panic("%! could not get a virtual range for mapping", "apic");

	// "APIC registers are memory-mapped to a 4-KByte region of the processor’s physical
	// address space with an initial starting address of FEE00000H." - x86 System Programming Manual, 10.4.1
	AddressSpace::map(virtualBase, physicalBase, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS | PAGE_CACHE_DISABLED);
}

/**
 *
 */
bool Lapic::isPrepared()
{
	return prepared;
}

/**
 *
 */
uint32_t Lapic::read(uint32_t reg)
{
	return *((volatile uint32_t*) (virtualBase + reg));
}

/**
 *
 */
void Lapic::write(uint32_t reg, uint32_t value)
{
	*((volatile uint32_t*) (virtualBase + reg)) = value;
}

/**
 *
 */
void Lapic::startTimer()
{
	logInfo("%! starting timer", "lapic");

	// Tell APIC timer to use divider 16
	write(APIC_REGISTER_TIMER_DIV, 0x3);

	// Prepare the PIT to sleep for 10ms (10000µs)
	Pit::prepareSleep(10000);

	// Set APIC init counter to -1
	write(APIC_REGISTER_TIMER_INITCNT, 0xFFFFFFFF);

	// Perform PIT-supported sleep
	Pit::performSleep();

	// Stop the APIC timer
	write(APIC_REGISTER_LVT_TIMER, APIC_LVT_INT_MASKED);

	// Now we know how often the APIC timer has ticked in 10ms
	uint32_t ticksPer10ms = 0xFFFFFFFF - read(APIC_REGISTER_TIMER_CURRCNT);

	// Start timer as periodic on IRQ 0
	write(APIC_REGISTER_LVT_TIMER, 32 | APIC_LVT_TIMER_MODE_PERIODIC);
	write(APIC_REGISTER_TIMER_DIV, 0x3);
	write(APIC_REGISTER_TIMER_INITCNT, ticksPer10ms);
}

/**
 *
 */
uint32_t Lapic::readId()
{
	return (read(APIC_REGISTER_ID) >> 24) & 0xFF;
}

/**
 *
 */
void Lapic::sendEoi()
{
	write(APIC_REGISTER_EOI, 0);
}

/**
 *
 */
void Lapic::waitForIcrSend()
{
	while (APIC_LVT_GET_DELIVERY_STATUS(read(APIC_REGISTER_INT_COMMAND_HIGH)) == APIC_ICR_DELIVS_SEND_PENDING) ;
}

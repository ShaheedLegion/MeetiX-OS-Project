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

#include <system/system.hpp>
#include <logger/logger.hpp>
#include <video/pretty_boot.hpp>
#include <system/acpi/acpi.hpp>
#include <system/acpi/madt.hpp>
#include <system/interrupts/pic.hpp>
#include <system/interrupts/lapic.hpp>
#include <system/interrupts/ioapic.hpp>
#include <system/interrupts/ioapic_manager.hpp>
#include <system/interrupts/descriptors/idt.hpp>
#include <system/smp/smp.hpp>
#include <kernel.hpp>
#include <system/processor.hpp>
#include <system/processor.hpp>

static Processor *processorList = 0;
static uint32_t processorsAvailable = 0;

/**
 *
 */
Processor *System::getProcessorList() 
{
	return processorList;
}

/**
 *
 */
uint32_t System::getNumberOfProcessors() 
{
	return processorsAvailable;
}

/**
 *
 */
uint32_t System::currentProcessorId() 
{
	return Lapic::readId();
}

/**
 *
 */
void System::initializeBasicSystemPackage(PhysicalAddress initialPageDirectoryPhysical) 
{
	// Check if the required CPU features are available
	if (Processor::supportsCpuid()) { logDebug("%! supports CPUID", "cpu"); }

	else { EvaKernel::panic("%! no CPUID support", "cpu"); }

	// Do some CPU info output
	Processor::printInformation();

	// Enable SSE if available
	checkAndEnableSSE();

	// APIC must be available
	if (Processor::hasFeature(CpuidStandardEdxFeature::APIC)) { logDebug("%! APIC available", "cpu"); }

	else { EvaKernel::panic("%! no APIC available", "cpu"); }

	// Gather ACPI information
	Acpi::gatherInformation();
	if (Acpi::hasEntries()) 
	{
		logDebug("%! is available", "acpi");

		// Parse the MADT
		AcpiEntry *cur = Acpi::getEntryWithSignature("APIC");
		
		// This creates the list of CPU's
		if (cur) Madt::parse(cur->header);

	} 

	else EvaKernel::panic("%! ACPI info not available", "system");

	// Initialize the interrupt controllers
	if (Lapic::isPrepared() && IoApicManager::areAvailable() && processorList) 
	{
		// Initialize the interrupt descriptor table
		Idt::prepare();
		Idt::load();

		// Disable PIC properly
		Pic::remapIrqs();
		Pic::maskAll();

		// Initialize local APIC
		Lapic::initialize();

		// Initialize each IO APIC
		IoApic *ioapic = IoApicManager::getEntries();
		while (ioapic) 
		{
			ioapic->initialize();
			ioapic = ioapic->getNext();
		}
		
		// Print available CPUs
		logInfo("%! %i available core%s", "system", processorsAvailable, (processorsAvailable > 1 ? "s" : ""));
		PRETTY_BOOT_STATUS("Initializing physical cores", 50, GREEN);

		// Initialize multiprocessing
		Smp::initialize(initialPageDirectoryPhysical);

		// Create keyboard and mouse redirection entries
		IoApicManager::createIsaRedirectionEntry(1, 1, 0);
		IoApicManager::createIsaRedirectionEntry(12, 12, 0);

	} 

	else EvaKernel::panic("%! pic compatibility mode not implemented. apic/ioapic required!", "system");
}

/**
 * 
 */
void System::initializeAdvancedPackage() 
{
	// Load interrupt descriptor table
	Idt::load();

	// Enable SSE if available
	checkAndEnableSSE();

	// Initialize local APIC
	Lapic::initialize();
}

/**
 *
 */
void System::checkAndEnableSSE() 
{

	if (Processor::hasFeature(CpuidStandardEdxFeature::SSE)) 
	{
		logInfo("%! support enabled", "sse");
		Processor::enableSSE();
	} 

	else logWarn("%! no support detected", "sse");
}

/**
 *
 */
void System::addProcessor(uint32_t apicId) 
{

	// Check if ID is in use
	Processor *current = processorList;
	while (current) 
	{
		if (current->apic == apicId) 
		{
			logWarn("%! ignoring core with irregular, duplicate id %i", "system", apicId);
			return;
		}
		current = current->next;
	}

	// Create core
	Processor *core = new Processor();
	core->apic = apicId;
	core->next = processorList;

	// This function is called by the BSP only, therefore we can do:
	if (apicId == Lapic::readId()) core->bsp = true;

	processorList = core;

	++processorsAvailable;
}

/**
 *
 */
Processor *System::getProcessorById(uint32_t coreId) 
{

	Processor *current = processorList;
	while (current) 
	{
		if (current->apic == coreId) return current;
		current = current->next;
	}

	return 0;
}


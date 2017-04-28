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

#include <executable/elf32_loader.hpp>

#include <kernel.hpp>
#include <logger/logger.hpp>
#include <ramdisk/ramdisk.hpp>
#include <tasking/tasking.hpp>
#include <tasking/thread_manager.hpp>
#include <memory/physical/pp_allocator.hpp>
#include <memory/physical/pp_reference_tracker.hpp>
#include <memory/paging.hpp>
#include <memory/address_space.hpp>
#include <memory/memory.hpp>
#include <memory/temporary_paging_util.hpp>
#include <memory/constants.hpp>
#include <utils/string.hpp>

/**
 * Spawns a ramdisk file as a process.
 */
Elf32SpawnStatus Elf32Loader::spawnFromRamdisk(RamdiskEntry *entry, SecurityLevel securityLevel, Thread **target, bool enforceCurrentCore, ThreadPriority priority) 
{
	// Check file
	if (entry == 0 || entry->type != RAMDISK_ENTRY_TYPE_FILE) 
	{
		return ELF32_SPAWN_STATUS_FILE_NOT_FOUND;
	}

	// Get and validate ELF header
	Elf32Ehdr *header = (Elf32Ehdr*) entry->data;
	Elf32ValidationStatus status = validate(header);

	if (status == ELF32_VALIDATION_SUCCESSFUL) 
	{
		// Create the process
		Thread *mainThread = ThreadManager::createProcess(securityLevel, nullptr);
		if (mainThread == 0) 
		{
			logWarn("%! failed to create main thread to spawn ELF binary from ramdisk", "elf32");
			return ELF32_SPAWN_STATUS_PROCESS_CREATION_FAILED;
		}
		Process *process = mainThread->process;

		// Temporarily switch to the new processes page directory to load the binary to it
		PageDirectory thisPageDirectory = AddressSpace::getCurrentSpace();

		AddressSpace::switchToSpace(process->pageDirectory);
		loadBinaryToCurrentAddressSpace(header, process);
		ThreadManager::prepareThreadLocalStorage(mainThread);
		AddressSpace::switchToSpace(thisPageDirectory);

		// Set the tasks entry point
		mainThread->cpuState->eip = header->e_entry;

		// Set priority
		mainThread->priority = priority;

		// Add to scheduling list
		Tasking::addTask(mainThread, enforceCurrentCore);

		// Set out parameter
		*target = mainThread;
		return ELF32_SPAWN_STATUS_SUCCESSFUL;
	}

	return ELF32_SPAWN_STATUS_VALIDATION_ERROR;
}

/**
 * This method loads the binary with the given header to the current address space. Once finished,
 * the start and end address of the executable image in memory are written to the respective out parameters.
 */
void Elf32Loader::loadBinaryToCurrentAddressSpace(Elf32Ehdr *header, Process *process) 
{
	loadLoadSegment(header, process);
	loadTlsMasterCopy(header, process);
}

/**
 *
 */
void Elf32Loader::loadLoadSegment(Elf32Ehdr *header, Process *process) 
{
	// Initial values
	uint32_t imageStart = 0xFFFFFFFF;
	uint32_t imageEnd = 0;

	// First find out how much place the image needs in memory
	for (uint32_t i = 0; i < header->e_phnum; i++) 
	{
		Elf32Phdr *programHeader = (Elf32Phdr*) (((uint32_t) header) + header->e_phoff + (header->e_phentsize * i));
		
		if (programHeader->p_type != PT_LOAD) continue;
		if (programHeader->p_vaddr < imageStart) imageStart = programHeader->p_vaddr;
		if (programHeader->p_vaddr + programHeader->p_memsz > imageEnd) imageEnd = programHeader->p_vaddr + programHeader->p_memsz;
	}

	// Align the addresses
	imageStart = PAGE_ALIGN_DOWN(imageStart);
	imageEnd = PAGE_ALIGN_UP(imageEnd);

	// Map pages for the executable
	for (uint32_t virt = imageStart; virt < imageEnd; virt = virt + PAGE_SIZE) 
	{
		uint32_t phys = PPallocator::allocate();
		AddressSpace::map(virt, phys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
		PPreferenceTracker::increment(phys);
	}

	// Write the image to memory
	for (uint32_t i = 0; i < header->e_phnum; i++) 
	{
		Elf32Phdr *programHeader = (Elf32Phdr*) (((uint32_t) header) + header->e_phoff + (header->e_phentsize * i));
		if (programHeader->p_type != PT_LOAD)
			continue;
		Memory::setBytes((void*) programHeader->p_vaddr, 0, programHeader->p_memsz);
		Memory::copy((void*) programHeader->p_vaddr, (uint8_t*) (((uint32_t) header) + programHeader->p_offset), programHeader->p_filesz);
	}

	// Set out parameters
	process->imageStart = imageStart;
	process->imageEnd = imageEnd;
}

/**
 *
 */
void Elf32Loader::loadTlsMasterCopy(Elf32Ehdr *header, Process *process) 
{
	uint32_t tlsSize = 0;

	// Map pages for TLS master copy
	for (uint32_t i = 0; i < header->e_phnum; i++) 
	{
		Elf32Phdr *programHeader = (Elf32Phdr*) (((uint32_t) header) + header->e_phoff + (header->e_phentsize * i));
		if (programHeader->p_type == PT_TLS) 
		{
			tlsSize = PAGE_ALIGN_UP(programHeader->p_memsz);

			uint32_t tlsPages = tlsSize / PAGE_SIZE;
			uint32_t tlsStart = process->virtualRanges.allocate(tlsPages);
			uint32_t tlsEnd = tlsStart + tlsSize;

			for (uint32_t virt = tlsStart; virt < tlsEnd; virt = virt + PAGE_SIZE) 
			{
				uint32_t phys = PPallocator::allocate();
				AddressSpace::map(virt, phys, DEFAULT_USER_TABLE_FLAGS, DEFAULT_USER_PAGE_FLAGS);
				PPreferenceTracker::increment(phys);
			}

			Memory::setBytes((void*) tlsStart, 0, programHeader->p_memsz);
			Memory::copy((void*) tlsStart, (uint8_t*) (((uint32_t) header) + programHeader->p_offset), programHeader->p_filesz);
			break;
		}
	}

}

/**
 * Validates if the executable with the given header can be run on this system.
 */
Elf32ValidationStatus Elf32Loader::validate(Elf32Ehdr *header) 
{
	// Valid ELF header
	if (/**/(header->e_ident[EI_MAG0] != ELFMAG0) || // 0x7F
			(header->e_ident[EI_MAG1] != ELFMAG1) || // E
			(header->e_ident[EI_MAG2] != ELFMAG2) || // L
			(header->e_ident[EI_MAG3] != ELFMAG3))   // F
			
			{
		return ELF32_VALIDATION_NOT_ELF;
	}

	// Must be executable
	if (header->e_type != ET_EXEC) return ELF32_VALIDATION_NOT_EXECUTABLE;

	// Must be i386 architecture compatible
	if (header->e_machine != EM_386) return ELF32_VALIDATION_NOT_I386;

	// Must be 32 bit
	if (header->e_ident[EI_CLASS] != ELFCLASS32) return ELF32_VALIDATION_NOT_32BIT;

	// Must be little endian
	if (header->e_ident[EI_DATA] != ELFDATA2LSB) return ELF32_VALIDATION_NOT_LITTLE_ENDIAN;

	// Must comply to current ELF standard
	if (header->e_version != EV_CURRENT) return ELF32_VALIDATION_NOT_STANDARD_ELF;

	// All fine
	return ELF32_VALIDATION_SUCCESSFUL;
}

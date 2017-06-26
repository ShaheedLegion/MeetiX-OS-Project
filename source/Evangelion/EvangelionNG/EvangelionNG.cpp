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

#include "EvangelionNG.hpp"
#include "kernelloader/SetupInformation.hpp"
#include "multiboot/MultibootUtil.hpp"

#include "eva/stdint.h"
#include "logger/logger.hpp"
#include "video/ConsoleVideo.hpp"
#include "video/PrettyBoot.hpp"

#include "system/system.hpp"
#include "system/smp/GlobalLock.hpp"
#include "system/serial/SerialPort.hpp"
#include "system/BiosDataArea.hpp"
#include "system/pci/pci.hpp"
#include "tasking/tasking.hpp"
#include "filesystem/filesystem.hpp"

#include "memory/gdt/GdtManager.hpp"
#include "memory/KernelHeap.hpp"
#include "memory/physical/PPallocator.hpp"
#include "memory/paging.hpp"
#include "memory/AddressSpace.hpp"
#include "memory/TemporaryPagingUtil.hpp"
#include "memory/LowerHeap.hpp"
#include "memory/constants.hpp"
#include "memory/collections/AddressStack.hpp"

#include "executable/Elf32Loader.hpp"
#include "memory/collections/AddressRangePool.hpp"

AddressRangePool *EvaKernel::evaKernelRangePool;
Ramdisk *EvaKernel::ramdisk;

// Locker per inizializzare il sistema su monocore e/o multicore disabilitando gli interrupts
static GlobalLock BspSetupLock;
static GlobalLock ApSetupLock;
static int coreNumber;

// bloccatore per il lancio delle applicazioni di base (idle e init)
static GlobalLock systemProcessSpawnLock;

/**
 * inizializza l'ambiente
 */
void EvaKernel::preSetup(SetupInformation *info)
{
	// abilita le porte COM (per i log del kernel)
	ComPortInformation comPortInfo = biosDataArea->comPortInfo;
	if (comPortInfo.com1 > 0)
	{
		SerialPort::initializePort(comPortInfo.com1, false); // Initialize in poll mode
		Logger::enableSerialPortLogging();
		DebugInterface::initialize(comPortInfo.com1);
	}

	else Logger::println("%! COM1 port not available for serial debug output", "logger");

	// stampa l'header con la versione (la funzione sta a riga 289)
	printHeader(info);
	PRETTY_BOOT_STATUS("Checking available memory", 20, GREEN);

	// inizializza l'allocatore di memoria con i valori passatigli dall'EvaLoader
	PPallocator::initializeFromBitmap(info->bitmapStart, info->bitmapEnd);

	// crea una variabile con la quantità di ram in mega partendo dalla disponibilità iniziale in pagine (4K)
	uint32_t mbs = (PPallocator::getInitialAmount() * PAGE_SIZE / 1024) / 1024;
	logInfo("%! memory: %iMB", "Eva Kernel", mbs);

	// crea uno stack dati per la cache di EvangelionNG
	KernelHeap::initialize(info->heapStart, info->heapEnd);

	// Ricerca nella memoria il file del ramdisk
	PRETTY_BOOT_STATUS("Searching ramdisk", 30, GREEN);
	MultibootModule *ramDiskModule = MultibootUtils::findModule(info->multibootInformation, "/boot/MXfs.img");
	if (ramDiskModule == 0) panic("%! ramdisk MXfs.img does not exist", "Eva Kernel");

	// crea i range di memoria di EvangelionNG
	PRETTY_BOOT_STATUS("Initializing kernel rande pool", 35, GREEN);
	EvaKernel::evaKernelRangePool = new AddressRangePool();
	EvaKernel::evaKernelRangePool->initialize(CONST_KERNEL_VIRTUAL_RANGES_START, CONST_KERNEL_VIRTUAL_RANGES_END);

	// carica il ramdisk e lo mappa nei ranges del kernel
	PRETTY_BOOT_STATUS("Loading ramdisk", 40, GREEN);
	loadRamdisk(ramDiskModule);
}

/**
 *	questa è la funzione principale da cui EvangelionNG parte, viene richiamata dall'EvaLoader
 */
void EvaKernel::run(SetupInformation *info)
{
	PRETTY_BOOT_STATUS("Running pre setup", 15, GREEN);
	// richiama la funzione sopra,
	preSetup(info);

	// recupera la memoria occupata dall EvaLoader
	PhysicalAddress initialPdPhysical = info->initialPageDirectoryPhysical;
	logDebug("%! unmapping old address space area", "Eva Kernel");
	PRETTY_BOOT_STATUS("Deallocating EvaLoader", 41, GREEN);
	for (VirtualAddress i = CONST_LOWER_MEMORY_END; i < CONST_KERNEL_AREA_START; i = i + PAGE_SIZE)
		AddressSpace::unmap(i);
	// NOTE: il puntatore a info è ora deallocato

	// lancia l'inizializzazione monocore
	runBasicSystemPackage(initialPdPhysical);
}

/**
 * avvio in monocore
 */
void EvaKernel::runBasicSystemPackage(PhysicalAddress initialPdPhysical)
{
	PRETTY_BOOT_STATUS("Initializing paging", 42, GREEN);
	// inizializza il paging
	TemporaryPagingUtil::initialize();

	PRETTY_BOOT_STATUS("Expanding to max kernel heap", 44, GREEN);
	// chiede se è possibile allocare ancora piu memoria
	LowerHeap::addArea(CONST_LOWER_HEAP_MEMORY_START, CONST_LOWER_HEAP_MEMORY_END);

	// lancia l'inizializzazione monocore
	BspSetupLock.lock();
	{
		// inizializza il core e gli Interrupts
		PRETTY_BOOT_STATUS("Initializing Basic System package", 45, GREEN);
		System::initializeBasicSystemPackage(initialPdPhysical);

		// inizializza il global descriptor table
		// (AFTER the system, so BSP's id is available)
		PRETTY_BOOT_STATUS("Initializing Global Descriptor Table", 50, GREEN);
		GdtManager::prepare();
		GdtManager::initialize();

		// inizializza il multitasking e lo scheduler
		PRETTY_BOOT_STATUS("Initializing multitasking", 70, GREEN);
		Tasking::initialize();

		// e lo abilita per il core corrente
		Tasking::enableForThisCore();

		// inizializza il filesystem
		PRETTY_BOOT_STATUS("Initializing FileSystem", 80, GREEN);
		FileSystem::initialize();

		// con la tecnica di BruteForce scansiona i bus pci
		PRETTY_BOOT_STATUS("Scanning pci devices", 85, GREEN);
		Pci::initialize();

		// lancia i processi inziali
		// idle mantiene il core a 0%
		PRETTY_BOOT_STATUS("Loading basic binaries", 95, GREEN);
		{
			PRETTY_BOOT_STATUS("Load Idle process", 97, GREEN);
			loadSystemProcess("/cmd/idle", THREAD_PRIORITY_IDLE);

			PRETTY_BOOT_STATUS("Load Init process", 99, GREEN);
			loadSystemProcess("/sys/eva/server/spawner.sv", THREAD_PRIORITY_NORMAL);
		}
		PRETTY_BOOT_STATUS("Starting Userspace", 100, GREEN);
	}
	BspSetupLock.unlock();
	/* finisce l'inizializzazione monocore */

	// ricerca se è disponibile piu di un core
	coreNumber = System::getNumberOfProcessors() - 1;
	logInfo("%! waiting for %i application processors", "Eva Kernel", coreNumber);
	while (coreNumber > 0)
	{
		// se si, con l'istruzione assembly (tutte le istruzioni sotto asm() sono assembly)
		//pause lancia a ripetizione la funzione run_ap sotto
		asm("pause");
	}

	// Abilita gli interrupt e aspetta con hlt un interrupt request che causerà l'innesco dello scheduler e l'avvio di init
	logInfo("%! leaving initialization", "Eva Kernel");
	asm("sti");
	while (true)
		asm("hlt");
}

/**
 * multicore
 */
void EvaKernel::runAdvancedSystemPackage()
{
	ApSetupLock.lock();
	{
		PRETTY_BOOT_STATUS("Initializing Advanged System package", 100, RED);

		// mostra il contenuto del registro esp
		uint32_t esp;
		asm("mov %%esp, %0":"=g"(esp));
		logInfo("%! esp is %h", esp, "Eva AP");

		// aspetta che l'inizializzazione monocore termini
		logInfo("%! waiting for bsp to finish setup", "Eva AP");

		// fa un log con lo stato dell'inizializzazione monocore
		BspSetupLock.lock();
		BspSetupLock.unlock();

		// inizializza il global descriptor table
		GdtManager::initialize();

		// inizializza il core corrente
		System::initializeAdvancedPackage();

		//e vi abilita il multitasking
		Tasking::enableForThisCore();

		// lancia il processo idle
		loadSystemProcess("/cmd/idle", THREAD_PRIORITY_IDLE);

		// diminuisce di uno i core da inizializzare
		--coreNumber;
	}
	ApSetupLock.unlock();

	// si rilancia a ciclo se è disponibile un altro core
	logInfo("%! waiting for %i application processors", "Eva AP", coreNumber);
	while (coreNumber > 0)
		asm("pause");

	// Abilita gli interrupt e aspetta con hlt un interrupt request che causerà l'innesco dello scheduler e l'avvio di init
	logInfo("%! leaving initialization", "Eva AP");
	asm("sti");
	while (true)
		asm("hlt");
}

/**
 * lancia i processi iniziali
 */
void EvaKernel::loadSystemProcess(const char *binaryPath, ThreadPriority priority)
{
	systemProcessSpawnLock.lock();

	Thread *systemProcess;
	Elf32SpawnStatus status = Elf32Loader::spawnFromRamdisk(binaryPath, SECURITY_LEVEL_KERNEL, &systemProcess, true, priority);
	switch (status)
	{
		case ELF32_SPAWN_STATUS_SUCCESSFUL: logInfo("%! successful spawned \"%s\"", "sysproc", binaryPath); break;
		case ELF32_SPAWN_STATUS_FILE_NOT_FOUND: panic("%! \"%s\" not found", "Eva Kernel", binaryPath); break;
		case ELF32_SPAWN_STATUS_VALIDATION_ERROR: panic("%! \"%s\" is not a valid elf32 binary", "Eva Kernel", binaryPath); break;
		case ELF32_SPAWN_STATUS_PROCESS_CREATION_FAILED: panic("%! \"%s\" could not be loaded, error creating process", "Eva Kernel", binaryPath); break;
		default: panic("%! \"%s\" could not be loaded", "Eva Kernel", binaryPath); break;
	}

	systemProcessSpawnLock.unlock();
}

/**
 * stampa il nome del Kernel e la versione
 */
void EvaKernel::printHeader(SetupInformation *info)
{
	// Print header
	ConsoleVideo::clear();
	ConsoleVideo::setColor(32);
	logInfon("EvangelionNG Kernel");
	ConsoleVideo::setColor(0x0F);
	logInfo(" Version %i.%i.%i.%c", VERSION_MAJOR, VERSION_MINOR, VERSION_SUB, VERSION_PATCH);
	logInfo("");
	logInfo("  Copyright (C) 2017, MeetiX OS Project");
	logInfo("");
	logInfo("%! loading", "Pre Setup");

	// Print setup information
	logDebug("%! setup information:", "Pre Setup");
	logDebug("%#   reserved: %h - %h", info->kernelImageStart, info->kernelImageEnd);
	logDebug("%#   stack:    %h - %h", info->stackStart, info->stackEnd);
	logDebug("%#   bitmap:   %h - %h", info->bitmapStart, info->bitmapEnd);
	logDebug("%#   heap:     %h - %h", info->heapStart, info->heapEnd);
	logDebug("%#   mbstruct: %h", info->multibootInformation);
	logDebug("%! started", "Eva Kernel");
	logDebug("%! got setup information at %h", "Eva Kernel", info);
}

/**
 * carica il ramdisk
 */
void EvaKernel::loadRamdisk(MultibootModule *ramdiskModule)
{
	// assegna alla variabile ramdiskPages la dimensione del ramdisk
	int ramdiskPages = PAGE_ALIGN_UP(ramdiskModule->moduleEnd - ramdiskModule->moduleStart) / PAGE_SIZE;

	// e ne alloca la memoria in sola lettura
	VirtualAddress ramdiskNewLocation = EvaKernel::evaKernelRangePool->allocate(ramdiskPages);

	// nel caso non vi sia abbastanza spazio richiama il panic
	if (ramdiskNewLocation == 0) panic("%! not enough virtual space for ramdisk remapping", "Eva Kernel");

	// ora mappa la memoria occupata dal ramdisk come partizione scrivibile
	for (int i = 0; i < ramdiskPages; i++)
	{
		VirtualAddress virt = ramdiskNewLocation + i * PAGE_SIZE;
		PhysicalAddress phys = AddressSpace::virtualToPhysical(ramdiskModule->moduleStart + i * PAGE_SIZE);
		AddressSpace::map(virt, phys, DEFAULT_KERNEL_TABLE_FLAGS, DEFAULT_KERNEL_PAGE_FLAGS);
	}

	// ne istanzia inizio e fine
	ramdiskModule->moduleEnd = ramdiskNewLocation + (ramdiskModule->moduleEnd - ramdiskModule->moduleStart);
	ramdiskModule->moduleStart = ramdiskNewLocation;

	// e vi crea sopra cartelle e file
	EvaKernel::ramdisk = new Ramdisk();
	EvaKernel::ramdisk->load(ramdiskModule);
	logInfo("%! ramdisk mounted", "Eva Kernel");
}

/**
 * scrive in rosso il messaggio passatogli per argomento e blocca il sistema permanentemente in caso di errore
 */
void EvaKernel::panic(const char *msg, ...)
{
	//blocca i log affinchè nessun altro processo possa scrivere un log
	Logger::manualLock();

	//stampa il messaggio
	logInfo("%*%! an unrecoverable error has occured. reason:", 0x0C, "Eva Error");

	va_list valist;
	va_start(valist, msg);
	Logger::printFormatted(msg, valist);
	va_end(valist);
	Logger::printCharacter('\n');

	Logger::manualUnlock();

	asm("cli");
	while (true)
		asm("hlt");
}

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

#ifndef EVA_EXECUTABLE_ELF32_LOADER
#define EVA_EXECUTABLE_ELF32_LOADER

#include "eva/elf32.h"
#include <ramdisk/ramdisk.hpp>
#include <memory/paging.hpp>
#include <tasking/thread.hpp>

/**
 * Executable spawn status
 */
enum Elf32SpawnStatus
{
	ELF32_SPAWN_STATUS_SUCCESSFUL,
	ELF32_SPAWN_STATUS_FILE_NOT_FOUND,
	ELF32_SPAWN_STATUS_VALIDATION_ERROR,
	ELF32_SPAWN_STATUS_PROCESS_CREATION_FAILED
};

/**
 * ELF validation status
 */
enum Elf32ValidationStatus
{
	ELF32_VALIDATION_SUCCESSFUL,
	ELF32_VALIDATION_NOT_ELF,
	ELF32_VALIDATION_NOT_EXECUTABLE,
	ELF32_VALIDATION_NOT_I386,
	ELF32_VALIDATION_NOT_32BIT,
	ELF32_VALIDATION_NOT_LITTLE_ENDIAN,
	ELF32_VALIDATION_NOT_STANDARD_ELF
};

/**
 * Executable loader for 32bit ELF binaries
 */
class Elf32Loader
{
public:
	static Elf32SpawnStatus spawnFromRamdisk(const char *path, SecurityLevel securityLevel, Thread **target, bool enforceCurrentCore, ThreadPriority priority);

private:
	static Elf32ValidationStatus validate(Elf32Ehdr *header);
	static void loadBinaryToCurrentAddressSpace(Elf32Ehdr *binaryHeader, Process *process);
	static void loadTlsMasterCopy(Elf32Ehdr *header, Process *process);
	static void loadLoadSegment(Elf32Ehdr *header, Process *process);
};

#endif

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

#ifndef EVA_KERNEL
#define EVA_KERNEL

#include <build_config.hpp>
#include <kernelloader/setup_information.hpp>
#include <memory/collections/address_range_pool.hpp>
#include <ramdisk/ramdisk.hpp>
#include <tasking/thread.hpp>

/**
 * main kernel class
 */
class EvaKernel 
{
public:
	/**
	 * Pool of virtual ranges used in the kernel to map memory.
	 */
	static AddressRangePool *evaKernelRangePool;

	/**
	 * Pointer to the global ramdisk instance.
	 */
	static Ramdisk *ramdisk;

	/**
	 * Does the initial setup of the kernel. The setup information struct is provided
	 * from the bootloader and contains important information like the position of the
	 * kernel image, or the memory bitmap.
	 *
	 * @param info the setup information provided by the kernel loader
	 */
	static void run(SetupInformation *info);

	/**
	 * BSP setup routine
	 */
	static void runBasicSystemPackage(PhysicalAddress initialPdPhysical);

	/**
	 * AP setup routine
	 */
	static void runAdvancedSystemPackage();

	/**
	 * Loads a system process binary as a process with the specified priority.
	 *
	 * @param path		the ramdisk path to the binary
	 * @param priority	the thread priority to assign
	 */
	static void loadSystemProcess(const char *path, ThreadPriority priority);

	/**
	 * Triggers a kernel panic, means halting the entire system and displaying the given
	 * message/diagnostic information.
	 *
	 * @param message	the panic message, written in a format that the g_logger class understands
	 * @param ...		variable arguments for the message
	 */
	static void panic(const char *message, ...);

private:
	/**
	 *
	 */
	static void printHeader(SetupInformation *info);

	/**
	 *
	 */
	static void loadRamdisk(MultibootModule *ramdiskModule);

	/**
	 * Performs the very first kernel setup, copying all necessary information that
	 * is provided by the loader.
	 */
	static void preSetup(SetupInformation *info);
};

#endif

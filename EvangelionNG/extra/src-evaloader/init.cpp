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

#include <loader.hpp>
#include <runtime/constructors.hpp>

#include <logger/logger.hpp>
#include <video/console_video.hpp>
#include <video/pretty_boot.hpp>
#include <system/serial/serial_port.hpp>
#include "system/bios_data_area.hpp"
#include "debug/debug_interface.hpp"

/**
 * Initialization function, called from the loader assembly. Checks the
 * multiboot magic number and then passes the multiboot structure to the
 * loader for further initialization.
 *
 * @param multibootStruct	the multiboot structure passed by GRUB
 * @param magicNumber		the magic number passed by GRUB
 */
extern "C" void initializeLoader(MultibootInformation *multibootStruct, uint32_t magicNumber) 
{

	Constructors::call();

	// initialize COM port
	ComPortInformation comPortInfo = biosDataArea->comPortInfo;
	if (comPortInfo.com1 > 0) 
	{
		SerialPort::initializePort(comPortInfo.com1, false); // Initialize in poll mode
		Logger::enableSerialPortLogging();
		DebugInterface::initialize(comPortInfo.com1);
	} 

	else 
	{
		Logger::println("%! COM1 port not available for serial debug output", "logger");
	}

	// Clear the console and print the header colored
	if (PRETTY_BOOT) 
	{
		PrettyBoot::enable();
	} 

	else 
	{
		ConsoleVideo::clear();
	}

	// Clear the console and print the header colored
	ConsoleVideo::clear();
	ConsoleVideo::setColor(32);
	logInfon("Evangelion Loader");
	ConsoleVideo::setColor(0x0F);
	logInfon(" Version %d.%d.%s", L_VERSION_MAJOR, L_VERSION_MINOR, L_VERSION_PATCH);
	logInfo("");

	logInfo("%! checking magic number", "early");

	if (magicNumber == MULTIBOOT_BOOTLOADER_MAGIC) 
	{
		logInfo("%! initializing loader", "early");
		EvaLoader::initialize(multibootStruct);

	} 

	else 
	{
		logInfo("%! invalid magic number in multiboot struct", "early");
	}
}

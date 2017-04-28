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

#include <kernel.hpp>
#include <runtime/constructors.hpp>

#include <build_config.hpp>
#include <kernelloader/setup_information.hpp>
#include <logger/logger.hpp>
#include <video/pretty_boot.hpp>
#include <video/console_video.hpp>
#include "eva/stdint.h"
#include <system/serial/serial_port.hpp>

/**
 * Does the final loading preparation and starts the kernel.
 *
 * @param setupInformation		the setup information passed by the loader
 */
extern "C" void loadKernel(SetupInformation *setupInformation) 
{
	Constructors::call();

	// Set video output
	if (PRETTY_BOOT) PrettyBoot::enable(false);
	else ConsoleVideo::clear();

	// Call the kernel
	EvaKernel::run(setupInformation);

	// Hang after execution
	logInfo("%! execution finished, halting", "EvaKernel");
	
	asm("cli");
	for (;;) 
		asm("hlt");
}

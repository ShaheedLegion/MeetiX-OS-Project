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

#include <multiboot/multiboot_module_analyzer.hpp>

#include <loader.hpp>
#include <logger/logger.hpp>

/**
 * 
 */
uint32_t MultibootModuleAnalyzer::calculateEndOfHighestModule() 
{

	logDebug("%! searching for modules", "multiboot");
	uint32_t highestEnd = 0;

	// Check if modules are available
	MultibootInformation *mbInfo = EvaLoader::getSetupInformation()->multibootInformation;
	if (mbInfo->flags & (1 << 3)) 
	{
		uint32_t moduleCount = mbInfo->modulesCount;
		for (uint32_t i = 0; i < moduleCount; i++) 
		{
			MultibootModule *module = (MultibootModule*) (mbInfo->modulesAddress + sizeof(MultibootModule) * i);
			logDebug("%#   %h - %h: %s ", module->moduleStart, module->moduleEnd, module->path);

			// Check modules end address
			if (module->moduleEnd > highestEnd) 
			{
				highestEnd = module->moduleEnd;
			}
		}

	} 

	else 
	{
		EvaLoader::panic("%! modules are not available", "multiboot");
	}

	logDebug("%! highest module end address is %h", "multiboot", highestEnd);
	return highestEnd;
}

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

#include <multiboot/MultibootUtil.hpp>
#include <logger/logger.hpp>
#include <utils/string.hpp>

/**
 *
 */
MultibootModule *MultibootUtils::findModule(MultibootInformation *info, const char *path)
{
	logDebug("%! searching for module: %s", "mbutil", path);

	uint32_t moduleCount = info->modulesCount;

	for (uint32_t i = 0; i < moduleCount; i++)
	{
		MultibootModule *module = (MultibootModule*) (info->modulesAddress + sizeof(MultibootModule) * i);
		logDebug("%#  module: %s", module->path);

		// Check modules path
		if (String::equals(module->path, path)) return module;
	}

	return 0;
}

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

#ifndef EVA_SHARED_VIDEO_PRETTYBOOT
#define EVA_SHARED_VIDEO_PRETTYBOOT

#include <stdarg.h>
#include "eva/stdint.h"
#include <build_config.hpp>

#if PRETTY_BOOT
#define PRETTY_BOOT_STATUS(text, percent, color)		PrettyBoot::updateStatus(text, percent, color)
#define PRETTY_BOOT_FAIL(text)							PrettyBoot::fail(text)

#else
#define PRETTY_BOOT_STATUS(text, percent, color)
#define PRETTY_BOOT_FAIL(text)
#define RED
#define GREEN
#endif

#define RED 	4
#define GREEN	32

/**
 *
 */
class PrettyBoot 
{
public:
	static void enable(bool clearScreen = true);

	static void printProgressBar(int percent, uint8_t color);
	static void printCentered(const char *string, int y, uint8_t color);

	static void updateStatus(const char *string, int percent, uint8_t color);
	static void fail(const char *string);
};

#endif

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

#ifndef EVA_SHARED_VIDEO_PRETTYBOOT
#define EVA_SHARED_VIDEO_PRETTYBOOT

#include <stdarg.h>
#include "eva/stdint.h"
#include <BuildConfig.hpp>

#if PRETTY_BOOT
#define PRETTY_BOOT_STATUS(text, percent, color)		PrettyBoot::updateStatus(text, percent, color)
#define PRETTY_BOOT_FAIL(text)							PrettyBoot::fail(text)
#define RED 	4
#define GREEN	32

#else
#define PRETTY_BOOT_STATUS(text, percent, color)        {}
#define PRETTY_BOOT_FAIL(text)                          {}
#define RED                                             0
#define GREEN                                           0
#endif

/**
 * the class provide methods to create a text progress bar on screen to show the system loading
 */
class PrettyBoot
{
private:
	/*
	 * print the progress bar on screen for percent with color
	 */
	static void printProgressBar(int percent, uint8_t color);

	/*
	 * print the provided string centered
	 */
	static void printCentered(const char *string, int y, uint8_t color);

public:
	/*
	 * initialize the screen for pretty boot
	 */
	static void enable(bool clearScreen = true);

	/*
	 * print the provided string and draw the bar with provided percent
	 */
	static void updateStatus(const char *string, int percent, uint8_t color);

	/*
	 * called in case of fail, print a red bar
	 */
	static void fail(const char *string);
};

#endif

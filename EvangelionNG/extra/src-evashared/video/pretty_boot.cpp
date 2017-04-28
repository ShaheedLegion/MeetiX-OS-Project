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

#include <video/pretty_boot.hpp>
#include <video/console_video.hpp>
#include <utils/string.hpp>
#include <memory/memory.hpp>
#include <logger/logger.hpp>

/**
 *
 */
void PrettyBoot::enable(bool clearScreen) 
{
	Logger::setVideo(false);
	if (clearScreen) 
	{
		ConsoleVideo::clear();
	}
	ConsoleVideo::setVisualCursor(-1, -1);
}

/**
 *	Print progress bar with provided color and provided percentage
 */
void PrettyBoot::printProgressBar(int percent, uint8_t color) 
{
	ConsoleVideo::putChar(8, 10, '\xC9', 0xFF);
	ConsoleVideo::putChar(8, 11, '\xC9', 0xFF);
	ConsoleVideo::putChar(8, 12, '\xC9', 0xFF);
	ConsoleVideo::putChar(8, 13, '\xC9', 0xFF);

	ConsoleVideo::putChar(71, 10, '\xC9', 0xFF);	
	ConsoleVideo::putChar(71, 11, '\xC9', 0xFF);
	ConsoleVideo::putChar(71, 12, '\xC9', 0xFF);
	ConsoleVideo::putChar(71, 13, '\xC9', 0xFF);

	for (int i = 9; i < 71; i++)
	{
		ConsoleVideo::putChar(i, 9, '-', 0xFF);
		ConsoleVideo::putChar(i, 14, '-', 0xFF);
	}


	for (int i = 10; i < 70; i++) 
	{
		ConsoleVideo::putChar(i, 11, ' ', 0x80);
		ConsoleVideo::putChar(i, 12, ' ', 0x80);
	}

	int cells = (60 * percent) / 100;

	for (int i = 10; i < 10 + cells; i++) 
	{
		ConsoleVideo::putChar(i, 11, ' ', color);
		ConsoleVideo::putChar(i, 12, ' ', color);
	}
}

/**
 *
 */
void PrettyBoot::printCentered(const char *string, int y, uint8_t color) 
{

	int strl = String::length(string);
	int leftBound = 40 - strl / 2;

	for (int i = 0; i < 80; i++) 
	{
		ConsoleVideo::putChar(i, y, ' ', 0x00);
	}
	ConsoleVideo::putString(leftBound, y, string, color);
}

/**
 *
 */
void PrettyBoot::updateStatus(const char *string, int percent, uint8_t color) 
{
	printCentered("Evangelion Kernel Loading...", 0, 32);
	printProgressBar(percent, color);
	printCentered(string, 20, 0x07);
}

/**
 *
 */
void PrettyBoot::fail(const char *string) 
{
	printCentered("Evangelion Kernel Loading...", 0, 32);
	printProgressBar(100, 0xC0);
	printCentered(string, 20, RED);
}

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

#include <video/ConsoleVideo.hpp>
#include <memory/memory.hpp>

static uint8_t *videoMemory = (uint8_t*) ConsoleVideo::VIDEO_MEMORY;
static uint8_t color = ConsoleVideo::DEFAULT_COLOR;
static uint32_t offset = 0;

/**
 *
 */
void ConsoleVideo::print(char c)
{
	if (c == '\n')
	{
		if (offset % (WIDTH * 2) == 0) print(' ');
		while (offset % (WIDTH * 2) != 0)
			print(' ');
	}

	else
	{
		videoMemory[offset++] = c;
		videoMemory[offset++] = color;

		if (offset >= (WIDTH * 2 * HEIGHT)) scrollUp();
	}
}

/**
 *
 */
void ConsoleVideo::putChar(uint16_t x, uint16_t y, char c, uint8_t color)
{
	videoMemory[y * (WIDTH * 2) + x * 2] = c;
	videoMemory[y * (WIDTH * 2) + x * 2 + 1] = color;
}

/**
 *
 */
void ConsoleVideo::putString(uint16_t x, uint16_t y, const char *c, uint8_t color)
{
	while (*c)
	{
		putChar(x++, y, *c, color);
		if (x > WIDTH)
		{
			x = 0;
			y++;
		}

		if (y > HEIGHT) y = 0;
		++c;
	}
}
/**
 *
 */
void ConsoleVideo::scrollUp()
{
	uint32_t screenBytesWithoutLastLine = SCREEN_BYTES - LINE_BYTES;

	Memory::copy(videoMemory, videoMemory + LINE_BYTES, screenBytesWithoutLastLine);
	offset -= LINE_BYTES;
	Memory::setWords(videoMemory + screenBytesWithoutLastLine, ((DEFAULT_COLOR << 8) | ' '), WIDTH);
}

/**
 *
 */
void ConsoleVideo::clear()
{
	for (uint32_t i = 0; i < 25; i++)
		print('\n');
	offset = 0;
}

/**
 *
 */
void ConsoleVideo::setColor(uint8_t newColor)
{
	color = newColor;
}

/**
 *
 */
void ConsoleVideo::setVisualCursor(int x, int y)
{
	uint16_t position = (y * WIDTH) + x;
	IOports::writeByte(0x3D4, 0x0F);
	IOports::writeByte(0x3D5, (uint8_t) (position & 0xFF));
	IOports::writeByte(0x3D4, 0x0E);
	IOports::writeByte(0x3D5, (uint8_t) ((position >> 8) & 0xFF));
}

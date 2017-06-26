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

#include <logger/logger.hpp>
#include <utils/string.hpp>
#include <video/ConsoleVideo.hpp>
#include <system/serial/SerialPort.hpp>
#include <system/BiosDataArea.hpp>
#include "debug/DebugInterface.hpp"

/**
 * Width of a log entry header
 */
static const uint32_t HEADER_WIDTH = 15;

/**
 * Logger variables
 */
static bool logSerial = false;
static bool logVideo = true;

/**
 *
 */
void Logger::enableSerialPortLogging()
{
	logSerial = true;
}

/**
 *
 */
void Logger::setSerial(bool serial)
{
	logSerial = serial;
}

/**
 *
 */
void Logger::setVideo(bool video)
{
	logVideo = video;
}

/**
 *
 */
void Logger::printFormatted(const char *constMessage, va_list valist)
{
	char *message = (char*) constMessage;

	uint16_t headerColor = 0x07;

	while (*message)
	{
		if (*message == '%')
		{
			++message;

			if (*message == 'i')
			{ // integer
				int32_t val = va_arg(valist, int32_t);
				printNumber(val, 10);
			}

			else if (*message == 'h')
			{ // positive hex number
				uint32_t val = va_arg(valist, uint32_t);
				printPlain("0x");
				printNumber(val, 16);
			}

			else if (*message == 'b')
			{ // boolean
				int val = va_arg(valist, int);
				printPlain((const char*) (val ? "true" : "false"));
			}

			else if (*message == 'c')
			{ // char
				int val = va_arg(valist, int);
				printCharacter((char) val);
			}

			else if (*message == 's')
			{ // string
				char *val = va_arg(valist, char*);
				printPlain(val);
			}

			else if (*message == '#')
			{ // indented printing
				for (uint32_t i = 0; i < HEADER_WIDTH + 2; i++)
					printCharacter(' ');
			}

			else if (*message == '!')
			{ // indented header printing
				char *val = va_arg(valist, char*);
				uint32_t headerlen = String::length(val);

				if (headerlen < HEADER_WIDTH)
				{
					for (uint32_t i = 0; i < HEADER_WIDTH - headerlen; i++)
						printCharacter(' ');
				}

				printCharacter('[');
				ConsoleVideo::setColor(headerColor);
				print(val);
				ConsoleVideo::setColor(0x0F);
				printCharacter(']');
			}

			else if (*message == '%')
			{ // escaped %
				printCharacter(*message);
			}

			else if (*message == '*')
			{ // header color change
				headerColor = (uint16_t) (va_arg(valist, int));
			}
		}

		else printCharacter(*message);
		++message;
	}
}

/**
 *
 */
void Logger::printNumber(uint32_t number, uint16_t base)
{
	// Remember if negative
	uint8_t negative = 0;
	if (base == 10)
	{
		negative = ((int32_t) number) < 0;

		if (negative) number = -number;
	}

	// Write chars in reverse order, not nullterminated
	char revbuf[32];

	char *cbufp = revbuf;
	int len = 0;
	do
	{
		*cbufp++ = "0123456789ABCDEF"[number % base];
		++len;
		number /= base;
	}
	while (number);

	// If base is 16, write 0's until 8
	if (base == 16)
	{
		while (len < 8)
		{
			*cbufp++ = '0';
			++len;
		}
	}

	// Reverse buffer
	char buf[len + 1];
	for (int i = 0; i < len; i++)
		buf[i] = revbuf[len - i - 1];
	buf[len] = 0;

	// Print number
	if (negative) printCharacter('-');
	printPlain(buf);
}

/**
 *
 */
void Logger::printPlain(const char *constMessage)
{
	char *message = (char*) constMessage;
	while (*message)
		printCharacter(*message++);
}

/**
 *
 */
void Logger::printCharacter(char c)
{
	if (logVideo) ConsoleVideo::print(c);
	if (logSerial) DebugInterface::writeLogCharacter(c);
}

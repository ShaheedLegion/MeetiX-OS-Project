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

#include "debug/debug_interface.hpp"
#include "system/serial/serial_port.hpp"

bool DebugInterfaceInitialized = false;
static uint16_t serialPort;

#if DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_FULL
static const int logBufferLength = 512;
static char logBuffer[logBufferLength];
static int logBuffered = 0;
#endif

/**
 *
 */
void DebugInterface::initialize(uint16_t port) 
{
	DebugInterfaceInitialized = true;
	serialPort = port;
}

/**
 *
 */
void DebugInterface::writeLogCharacter(char c) 
{

	if (!DebugInterfaceInitialized) 
	{
		return;
	}

#if DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_PLAIN_LOG
	SerialPort::write(serialPort, c);

#elif DEBUG_INTERFACE_MODE == DEBUG_INTERFACE_MODE_FULL

	// put char in buffer
	logBuffer[logBuffered++] = c;

	// write buffer to output on newline or full buffer
	if (c == '\n' || logBuffered == logBufferLength - 1) 
	{

		writeShort(G_DEBUG_MESSAGE_LOG);

		for (int i = 0; i < logBuffered; i++) 
		{
			SerialPort::write(serialPort, logBuffer[i]);
		}
		SerialPort::write(serialPort, 0);

		logBuffered = 0;
	}
#endif
}

/**
 *
 */
void DebugInterface::writeByte(uint8_t value) 
{

	SerialPort::write(serialPort, value);
}

/**
 *
 */
void DebugInterface::writeShort(uint16_t value) 
{

	SerialPort::write(serialPort, (value >> 0) & 0xFF);
	SerialPort::write(serialPort, (value >> 8) & 0xFF);
}

/**
 *
 */
void DebugInterface::writeInt(uint32_t value) 
{

	SerialPort::write(serialPort, (value >> 0) & 0xFF);
	SerialPort::write(serialPort, (value >> 8) & 0xFF);
	SerialPort::write(serialPort, (value >> 16) & 0xFF);
	SerialPort::write(serialPort, (value >> 24) & 0xFF);
}

/**
 *
 */
void DebugInterface::writeLong(uint64_t value) 
{

	SerialPort::write(serialPort, (value >> 0) & 0xFF);
	SerialPort::write(serialPort, (value >> 8) & 0xFF);
	SerialPort::write(serialPort, (value >> 16) & 0xFF);
	SerialPort::write(serialPort, (value >> 24) & 0xFF);
	SerialPort::write(serialPort, (value >> 32) & 0xFF);
	SerialPort::write(serialPort, (value >> 40) & 0xFF);
	SerialPort::write(serialPort, (value >> 48) & 0xFF);
	SerialPort::write(serialPort, (value >> 56) & 0xFF);
}

/**
 *
 */
void DebugInterface::writeString(const char *string) 
{

	const char* p = string;
	while (*p) 
	{
		SerialPort::write(serialPort, *p++);
	}
	SerialPort::write(serialPort, 0);
}

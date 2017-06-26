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
#include <system/timing/pit.hpp>
#include <system/IOPorts.hpp>
#include <logger/logger.hpp>

static uint32_t timerClocking;
static uint32_t sleepDivisor;

/**
 *
 */
void Pit::prepareSleep(uint32_t microseconds)
{
	// Sanity check
	if (microseconds > 54 * 1000)
	{
		logInfo("%! illegal use of sleep. may only sleep up to 54000 microseconds", "pit");
	}

	// Disable the speaker
	uint8_t speakerControlByte = IOports::readByte(0x61);
	speakerControlByte &= ~2; // set bit 1 to 0 (disables speaker output)
	IOports::writeByte(0x61, speakerControlByte);

	// Initialize PIT
	IOports::writeByte(0x43, PIT_CHANNEL_2 | PIT_OPMODE_0_IOTC | PIT_ACCESS_LOHIBYTE);

	// Configure PIT, calculate divisor for the requested microseconds
	sleepDivisor = PIT_FREQUENCY / (1000000 / microseconds);
}

/**
 *
 */
void Pit::performSleep()
{
	// Write the prepared sleep divisor
	IOports::writeByte(0x42, sleepDivisor & 0xFF);
	IOports::writeByte(0x42, sleepDivisor >> 8);

	// Reset the PIT counter and let it start
	uint8_t pitControlByte = IOports::readByte(0x61);
	IOports::writeByte(0x61, (uint8_t) pitControlByte & ~1);	// clear bit 0
	IOports::writeByte(0x61, (uint8_t) pitControlByte | 1);		// set bit 0

	// Wait for PIT counter to reach 0
	while (!(IOports::readByte(0x61) & 0x20)) {}
}

/**
 * Sets the timer clocking
 */
void Pit::startAsTimer(uint32_t hz)
{
	logInfo("%! started as timer on %i hertz", "pit", hz);

	timerClocking = hz;
	uint32_t divisor = PIT_FREQUENCY / hz; // Calculate the divisor
	IOports::writeByte(0x43, PIT_CHANNEL_0 | PIT_ACCESS_LOHIBYTE | PIT_OPMODE_3_SQUARE_WAV);
	IOports::writeByte(0x40, divisor & 0xFF); // Set low byte of the divisor
	IOports::writeByte(0x40, divisor >> 8); // Set high byte of the divisor
}

/**
 * Returns the last hz set as clocking
 */
uint32_t Pit::getTimerClocking()
{
	return timerClocking;
}

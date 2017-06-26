/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
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

#ifndef _SOUND_HPP_
#define _SOUND_HPP_

#include <stdint.h>

class Sound
{
public:
	Sound();

	/*
	 * make the audio beep at frequency for milliseconds time
	 */
	static void beep(int frequency, int milliseconds);

	/*
	 * Main sound driver
	 */
	static void sound(uint32_t nFrequence);

	/*
	 * set pit to 0 and shutdown speakers
	 */
	static void noSound();

private:
	#define SpeakerCtrl 0x43
	#define SpeakerData 0x42
};

#endif

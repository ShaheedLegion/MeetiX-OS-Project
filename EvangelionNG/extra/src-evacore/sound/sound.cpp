/*
 * sound.cpp
 *
 * Copyright (C) 2016 MeetiX OS Project
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 *
 */

#include "sound.hpp"
#include <system/io_ports.hpp>

//Play sound using built in speaker
void Sound::sound(uint32_t nFrequence)
{
    uint32_t Div;
    uint8_t status;

    //Set the PIT to the desired frequency
    Div = 1193180 / nFrequence;
    IOports::writeByte(SpeakerCtrl, 0xb6);
    IOports::writeByte(SpeakerData, (uint8_t) (Div) );
    IOports::writeByte(SpeakerData, (uint8_t) (Div >> 8));

    //And play the sound using the PC speaker
    status = IOports::readByte(0x61);

    if(status != (status | 3))
    {
        IOports::writeByte(0x61, status | 3);
    }
}

//make it shutup
void Sound::noSound()
{
 	uint8_t tmp = (IOports::readByte(0x61) & 0xFC);

 	IOports::writeByte(0x61, tmp);
}

//Make a beep
void Sound::beep(int frequency, int milliseconds)
{
    sound(frequency);

    for (int i = 0; i < milliseconds * 0x1000; i ++); 

    noSound();
}
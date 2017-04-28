/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "input_receiver.hpp"
#include <eva.h>
#include <mxuser/io/keyboard.hpp>
#include <mxuser/io/mouse.hpp>
#include <components/cursor.hpp>

/**
 *
 */
void InputReceiver_t::initialize() 
{
	CreateThread((void*) InputReceiver_t::startReceiveMouseEvents);
	CreateThread((void*) InputReceiver_t::startReceiveKeyEvents);
}

/**
 *
 */
void InputReceiver_t::startReceiveKeyEvents() 
{
	TaskRegisterID("zipNET/keyReceiver");

	EventProcessor_t *eventQueue = ZipNET::instance()->eventProcessor;

	KeyInfo info;
	while (true) 
	{
		info = Keyboard::readKey();
		eventQueue->bufferKeyEvent(info);

		ZipNET::instance()->triggerRender();
	}
}

/**
 *
 */
void InputReceiver_t::startReceiveMouseEvents() 
{
	TaskRegisterID("zipNET/mouseReceiver");
	ZipNET *instance = ZipNET::instance();
	EventProcessor_t *eventQueue = instance->eventProcessor;
	Dimension resolution = instance->videoOutput->getResolution();

	MouseInfo info;
	while (true) 
	{
		info = Mouse::readMouse();

		Cursor_t::nextPosition.x += info.x;
		Cursor_t::nextPosition.y -= info.y;

		if (Cursor_t::nextPosition.x < 0) Cursor_t::nextPosition.x = 0;
		if (Cursor_t::nextPosition.x > resolution.width - 2) Cursor_t::nextPosition.x = resolution.width - 2;
		if (Cursor_t::nextPosition.y < 0) Cursor_t::nextPosition.y = 0;
		if (Cursor_t::nextPosition.y > resolution.height - 2) Cursor_t::nextPosition.y = resolution.height - 2;

		Cursor_t::nextPressedButtons = MOUSE_BUTTON_NONE;
		if (info.button1) Cursor_t::nextPressedButtons |= MOUSE_BUTTON_1;
		if (info.button2) Cursor_t::nextPressedButtons |= MOUSE_BUTTON_2;
		if (info.button3) Cursor_t::nextPressedButtons |= MOUSE_BUTTON_3;

		ZipNET::instance()->triggerRender();
	}
}
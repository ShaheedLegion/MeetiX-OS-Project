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

#ifndef __WINDOWSERVER__
#define __WINDOWSERVER__

#include <components/component.hpp>
#include <components/screen.hpp>
#include <events/event_processor.hpp>
#include "output/video_output.hpp"
#include <mxuser/graphics/graphics.hpp>
#include "interface/command_message_responder_thread.hpp"

/**
 *
 */
class ZipNET 
{
public:
	VideoOutput_t *videoOutput;
	EventProcessor_t *eventProcessor;
	Screen_t *screen;
	CommandMessageResponderThread_t *responderThread;
	uint8_t renderAtom;
	Graphics global;

	/**
	 * Sets up the windowing system by configuring a video output, setting up the
	 * event processor and running the main loop. Each step of the main loop includes
	 * a event handling and rendering sequence.
	 */
	void launch();

	/**
	 *
	 */
	void mainLoop(Rectangle screenBounds);

	/**
	 * Blits the component state.
	 */
	void blit(Graphics *graphics);

	/**
	 * Dispatches the given event to the component.
	 *
	 * @return whether the event was handled
	 */
	bool dispatch(Component_t *component, Event_t &event);

	/**
	 * Dispatches the given event upwards the component tree.
	 */
	Component_t *dispatchUpwards(Component_t *component, Event_t &event);

	/**
	 * Returns the singleton instance of the window server.
	 *
	 * @return the instance
	 */
	static ZipNET *instance();

	/**
	 *
	 */
	void loadCursor();

	/**
	 *
	 */
	void triggerRender();

	/**
	 *
	 */
	void cleanup(Pid process);
};

#endif

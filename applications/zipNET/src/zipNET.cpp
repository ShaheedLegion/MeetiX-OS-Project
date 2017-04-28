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

#include "zipNET.hpp"
#include "output/vbe_video_output.hpp"
#include "input/input_receiver.hpp"
#include "events/event.hpp"
#include "events/locatable.hpp"
#include "interface/registration_thread.hpp"

#include <interface/component_registry.hpp>
#include <components/background.hpp>
#include <components/cursor.hpp>
#include <components/window.hpp>
#include <components/checkbox.hpp>
#include <components/text/text_field.hpp>
#include <components/scrollpane.hpp>
#include <components/scrollbar.hpp>
#include <components/button.hpp>
#include <layout/flow_layout_manager.hpp>
#include <layout/grid_layout_manager.hpp>

#include <iostream>
#include <map>

#include <mxuser/tasking/lock.hpp>
#include <mxuser/io/files/file_log.hpp>

#include <cairo/cairo.h>

// global instance of ZipNET class
static ZipNET *server;

// thread locker
static Lock dispatchLock;

// flag with last render time
static uint64_t renderStart;
static Label_t *taskLabel;

/**
 *
 */
int main(int argc, char *argv[]) 
{
	server = new ZipNET();
	server->launch();
	return 0;
}

/**
 *
 */
void ZipNET::launch() 
{
	// disable video log
	SetVideoLog(false);

	// initialize the video output
	videoOutput = new VbeVideoOutput_t();
	if (!videoOutput->initialize()) 
	{
		std::cerr << "failed to initialize video mode" << std::endl;
		FileLog::flog("failed to initialize video mode");
		return;
	}

	// set up event handling
	eventProcessor = new EventProcessor_t();
	InputReceiver_t::initialize();


	std::string keyLayout = "it-EU";
	FileLog::flog(("loading keyboard layout '" + keyLayout + "'").c_str());
	if (!Keyboard::loadLayout(keyLayout)) FileLog::flog(("failed to load keyboard layout '" + keyLayout + "', no keyboard input available").c_str());

	// create the cursor
	loadCursor();

	// get resolution
	Dimension resolution = videoOutput->getResolution();
	Rectangle screenBounds(0, 0, resolution.width, resolution.height);

	// instantiate screen object with bounds
	screen = new Screen_t();
	screen->setBounds(screenBounds);

	// add background to screen
	Background_t *background = new Background_t(screenBounds);
	screen->addChild(background);

	// start registration interface
	RegistrationThread_t *registrationThread = new RegistrationThread_t();
	registrationThread->start();

	// start responder interface
	responderThread = new CommandMessageResponderThread_t();
	responderThread->start();

	taskLabel = new Label_t();
	taskLabel->setBounds(Rectangle(75, resolution.height - 30, resolution.width - 155, 30));
	screen->addChild(taskLabel);

	// execute the main loop
	mainLoop(screenBounds);
}

/**
 *
 */
void lockCheck() 
{
	TaskRegisterID("zipNET/LockCheck");
	
	while (true) 
	{
		if (Millis() - renderStart > 3000) FileLog::flog("window server has frozen");

		Sleep(1000);
	}
}

/**
 *
 */
void ZipNET::mainLoop(Rectangle screenBounds) 
{
	global.resize(screenBounds.width, screenBounds.height);
	
	CreateThread((void*) lockCheck);
	
	Cursor_t::nextPosition = Point(screenBounds.width / 2, screenBounds.height / 2);

	// intially set rendering atom
	renderAtom = true;

	uint64_t renderTime;

	while (true) 
	{
		renderStart = Millis();
		eventProcessor->processMouseState();
		eventProcessor->process();

		// make the root component resolve all requirements
		screen->resolveRequirement(COMPONENT_REQUIREMENT_UPDATE);
		screen->resolveRequirement(COMPONENT_REQUIREMENT_LAYOUT);
		screen->resolveRequirement(COMPONENT_REQUIREMENT_PAINT);

		// blit the root component to the buffer
		screen->blit(&global, screenBounds, Point(0, 0));

		// paint the cursor
		Cursor_t::paint(&global);

		// blit output
		blit(&global);

		// limit to 60 fps
		renderTime = Millis() - renderStart;
		if (renderTime < (1000 / 60)) Sleep((1000 / 60) - renderTime);

		// wait for next rendering
		AtomicLock(&renderAtom);
	}
}

/**
 *
 */
void ZipNET::blit(Graphics *graphics) 
{
	Dimension resolution = videoOutput->getResolution();
	Rectangle screenBounds(0, 0, resolution.width, resolution.height);
	ColorArgb *buffer = (ColorArgb*) cairo_image_surface_get_data(graphics->getSurface());

	// get invalid output
	Rectangle invalid = screen->grabInvalid();

	if (invalid.width == 0 && invalid.height == 0) return;

	videoOutput->blit(invalid, screenBounds, buffer);
}

/**
 *
 */
void ZipNET::loadCursor() 
{
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/default.cursor")) FileLog::flog("Cursor not loaded1");
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/text.cursor")) FileLog::flog("Cursor not loaded2");
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/resize-ns.cursor")) FileLog::flog("Cursor not loaded3");
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/resize-ew.cursor")) FileLog::flog("Cursor not loaded4");
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/resize-nesw.cursor")) FileLog::flog("Cursor not loaded5");
	if (!Cursor_t::load("/sys/graphics/zipNET/cursor/resize-nwes.cursor")) FileLog::flog("Cursor not loaded6");
	
	Cursor_t::set("default");
	Cursor_t::focusedComponent = screen;
}

/**
 *
 */
Component_t *ZipNET::dispatchUpwards(Component_t *component, Event_t &event) 
{
	// store when dispatching to parents
	Point initialPosition;
	Locatable_t *locatable = dynamic_cast<Locatable_t*>(&event);
	if (locatable) initialPosition = locatable->position;

	// check upwards until someone accepts the event
	Component_t *acceptor = component;
	while (!dispatch(acceptor, event)) 
	{
		acceptor = acceptor->getParent();
		if (acceptor == 0) break;

		// restore position on locatable events
		if (locatable) locatable->position = initialPosition;
	}
	return acceptor;
}

/**
 *
 */
bool ZipNET::dispatch(Component_t *component, Event_t &event) 
{
	dispatchLock.lock();

	bool handled = false;

	if (component->canHandleEvents()) 
	{
		Locatable_t *locatable = dynamic_cast<Locatable_t*>(&event);
		if (locatable != 0) 
		{
			Point locationOnScreen = component->getLocationOnScreen();
			locatable->position.x -= locationOnScreen.x;
			locatable->position.y -= locationOnScreen.y;
		}

		handled = component->handle(event);
	}

	dispatchLock.unlock();
	return handled;
}

/**
 *
 */
ZipNET *ZipNET::instance() 
{
	return server;
}

/**
 *
 */
void ZipNET::triggerRender() 
{
	renderAtom = false;
}

/**
 *
 */
void ZipNET::cleanup(Pid process) 
{
	// get components mapped for process
	std::map<int, Component_t*> *components = ComponentRegistry_t::getProcessMap(process);

	// if component exist set false visible to clear screen
	if (components) 
	{
		for (std::pair<const int, Component_t*> &entry : *components) 
		{
			Component_t *component = entry.second;
			if (component) component->setVisible(false);
		}
	}
}
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

#include <components/cursor.hpp>

#include <eva.h>
#include <events/mouse_event.hpp>
#include <mxuser/utils/property_file_parser.hpp>
#include <mxuser/io/files/file_log.hpp>
#include <mxuser/utils/logger.hpp>
#include <zipNET.hpp>

using namespace std;

static map<string, CursorConfiguration> cursorConfigurations;
static CursorConfiguration *currentConfiguration = 0;

static cairo_t *cr;
static Screen_t *screen;

Point Cursor_t::position;
Point Cursor_t::nextPosition;

MouseButton Cursor_t::pressedButtons = MOUSE_EVENT_NONE;
MouseButton Cursor_t::nextPressedButtons = MOUSE_EVENT_NONE;
Component_t *Cursor_t::draggedComponent = 0;
Component_t *Cursor_t::hoveredComponent = 0;
Component_t *Cursor_t::focusedComponent = 0;

/**
 *
 */
void Cursor_t::set(string name) 
{

	if (cursorConfigurations.count(name) > 0) currentConfiguration = &cursorConfigurations[name];

	else if (cursorConfigurations.count("default") > 0) currentConfiguration = &cursorConfigurations["default"];

	else FileLog::flog("could neither load '" + name + "' cursor nor 'default' cursor");

	screen = ZipNET::instance()->screen;
	if (screen) screen->markDirty(getArea());

}

/**
 *
 */
bool Cursor_t::load(string cursorPath) 
{

	// Open config file
	ifstream in(cursorPath + "/cursor.cfg");
	if (!in.good()) return false;
	
	PropertyFileParser properties(in);
	map<string, string> content = properties.getProperties();

	// Read required params
	string name = content["name"];
	string hitpointX = content["hitpoint.x"];
	string hitpointY = content["hitpoint.y"];
	string image = content["image"];

	if (name.empty() || hitpointX.empty() || hitpointY.empty() || image.empty()) return false;

	// Convert hitpoint
	stringstream stx;
	stx << hitpointX;
	int hPointX;
	stx >> hPointX;

	stringstream sty;
	sty << hitpointY;
	int hPointY;
	sty >> hPointY;

	string cursorImagePath = (cursorPath + "/" + image);

	// check if file exists
	Fd cursorImageFile = Open(cursorImagePath.c_str());
	if (cursorImageFile == FD_NONE)
	{
		return false;
	}
	Close(cursorImageFile);

	// load cursor
	CursorConfiguration pack;
	pack.surface = cairo_image_surface_create_from_png(cursorImagePath.c_str());
	if (pack.surface == nullptr) 
	{
		stringstream out;
		out << "failed to load cursor image at " << cursorImagePath << " for configuration " << cursorPath;
		FileLog::flog(out.str());
		
		return false;
	}

	pack.hitpoint = Point(hPointX, hPointY);
	pack.size = Dimension(cairo_image_surface_get_width(pack.surface), cairo_image_surface_get_height(pack.surface));
	cursorConfigurations[name] = pack;

	return true;
}

/**
 *
 */
void Cursor_t::paint(Graphics *global) 
{

	cr = global->getContext();
	cairo_reset_clip(cr);

	if (currentConfiguration) 
	{
		// draw cursor image
		cairo_set_source_surface(cr, currentConfiguration->surface, position.x - currentConfiguration->hitpoint.x, position.y - currentConfiguration->hitpoint.y);
		cairo_paint(cr);
	} 

	else 
	{
		// draw fallback cursor
		cairo_set_source_rgb(cr, 0, 0, 0);
		cairo_rectangle(cr, position.x, position.y, FALLBACK_CURSOR_SIZE, FALLBACK_CURSOR_SIZE);
		cairo_fill(cr);
	}
}

/**
 *
 */
Rectangle Cursor_t::getArea() 
{

	// get area for current cursor
	if (currentConfiguration) return Rectangle(position.x - currentConfiguration->hitpoint.x, position.y - currentConfiguration->hitpoint.y, currentConfiguration->size.width, currentConfiguration->size.height);

	// fallback cursor is just a square
	return Rectangle(position.x, position.y, FALLBACK_CURSOR_SIZE, FALLBACK_CURSOR_SIZE);
}

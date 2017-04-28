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

#ifndef __CURSOR__
#define __CURSOR__

#include <components/component.hpp>
#include <events/mouse_event.hpp>
#include <mxuser/graphics/metrics/point.hpp>
#include <mxuser/ui/interface_specification.hpp>

#include <string.h>
#include <cstdio>
#include <sstream>
#include <map>
#include <fstream>
#include <cairo/cairo.h>

#define FALLBACK_CURSOR_SIZE	10

/**
 *
 */
struct CursorConfiguration 
{
	cairo_surface_t *surface;
	Point hitpoint;
	Dimension size;
};

/**
 *
 */
class Cursor_t 
{
public:
	static Point position;
	static Point nextPosition;
	static MouseButton pressedButtons;
	static MouseButton nextPressedButtons;

	static Component_t *draggedComponent;
	static Component_t *hoveredComponent;
	static Component_t *focusedComponent;

	/**
	 *
	 */
	static void paint(Graphics *global);

	/**
	 *
	 */
	static Rectangle getArea();

	/**
	 *
	 */
	static void set(std::string name);

	/**
	 *
	 */
	static bool load(std::string cursorPath);

};

#endif

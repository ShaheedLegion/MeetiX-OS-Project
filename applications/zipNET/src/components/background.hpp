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

#ifndef __BACKGROUND__
#define __BACKGROUND__

#include <components/component.hpp>
#include <mxuser/graphics/metrics/rectangle.hpp>

/**
 *
 */
class Background_t: public Component_t 
{
public:
	/**
	 * Instantiates a background that spans over the given bounds.
	 */
	Background_t(Rectangle bounds);
	virtual ~Background_t() { }

	/**
	 * Components
	 */
	virtual void paint();
	virtual bool handle(Event_t &e);
	virtual void setFocus(bool focus) {}
	
	/*
	 * Background
	 */
	void setBackground(std::string path);

private:
	cairo_surface_t *surface;
	std::string path_t;
};

#endif

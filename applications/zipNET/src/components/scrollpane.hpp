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

#ifndef __SCROLLPANE__
#define __SCROLLPANE__

#include <components/component.hpp>
#include <components/scrollbar.hpp>
#include <components/panel.hpp>

/**
 *
 */
class Scrollpane_t: public Component_t, public ScrollHandler_t 
{
private:
	Component_t *viewPort;
	Point scrollPosition;
	Scrollbar_t verticalScrollbar;
	Scrollbar_t horizontalScrollbar;

public:
	Scrollpane_t() : scrollPosition(Point(0, 0)), viewPort(nullptr), verticalScrollbar(ScrollbarOrientation_t::VERTICAL), horizontalScrollbar(ScrollbarOrientation_t::HORIZONTAL) {}

	virtual Point getPosition() const { return scrollPosition; }
	virtual void layout();
	virtual void setPosition(Point &position);
	virtual void setViewPort(Component_t *content);
	virtual Component_t *getViewPort() const { return viewPort; }
	virtual void handleScroll(Scrollbar_t *bar);

};

#endif

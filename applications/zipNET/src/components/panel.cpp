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

#include <components/panel.hpp>

/**
 *
 */
void Panel_t::paint() 
{
	cr = graphics.getContext();
	cairo_set_source_rgba(cr, ARGB_FR_FROM(shapeColor), ARGB_FG_FROM(shapeColor), ARGB_FB_FROM(shapeColor), ARGB_FA_FROM(shapeColor));
	cairo_rectangle(cr, 0, 0, getBounds().width, getBounds().height);
	cairo_fill(cr);
}

/**
 *
 */
void Panel_t::setBackground(ColorArgb color) 
{
	setColor(color, 0);
	markFor(COMPONENT_REQUIREMENT_PAINT);
}
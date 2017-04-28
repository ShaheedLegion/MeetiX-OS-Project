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

#include <components/checkbox.hpp>
#include <events/mouse_event.hpp>

/**
 *
 */
Checkbox_t::Checkbox_t() : Component_t(true), checked(false), boxSize(DEFAULT_BOX_SIZE), boxTextGap(DEFAULT_BOX_TEXT_GAP), hovered(false), pressed(false) 
{
	addChild(&label);
}

/**
 *
 */
void Checkbox_t::layout() 
{
	Dimension preferredSize = label.getPreferredSize();
	if (preferredSize.height < boxSize + boxTextGap) 
	{
		preferredSize.height = boxSize + boxTextGap;
	}
	preferredSize.width += preferredSize.height;
	setPreferredSize(preferredSize);
}

/**
 *
 */
void Checkbox_t::paint() 
{
	bounds = getBounds();
	surface = graphics.getContext();
	clearSurface();

	if (hovered) cairo_set_source_rgba(surface, COLOR_ARGB_TO_FPARAMS(ARGB(255, 200, 200, 200)));

	else cairo_set_source_rgba(surface, COLOR_ARGB_TO_FPARAMS(shapeColor));

	cairo_rectangle(surface, bounds.x, bounds.y, bounds.width, bounds.height);
	cairo_fill(surface);

	if (checked)
	{
		microSurface = graphics.getContext();

		cairo_set_source_rgba(microSurface, COLOR_ARGB_TO_FPARAMS(ARGB(255, 0, 200, 0)));
		cairo_rectangle(microSurface, bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4);
	}
}

/**
 *
 */
bool Checkbox_t::handle(Event_t &event) 
{

	MouseEvent_t *mouseEvent = dynamic_cast<MouseEvent_t*>(&event);
	if (mouseEvent) 
	{
		if (mouseEvent->type == MOUSE_EVENT_ENTER) 
		{
			hovered = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);

		} 

		else if (mouseEvent->type == MOUSE_EVENT_LEAVE) 
		{
			hovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);

		} 

		else if (mouseEvent->type == MOUSE_EVENT_PRESS) 
		{
			pressed = true;
			markFor(COMPONENT_REQUIREMENT_PAINT);

		} 

		else if (mouseEvent->type == MOUSE_EVENT_RELEASE || mouseEvent->type == MOUSE_EVENT_DRAG_RELEASE) 
		{
			pressed = false;

			Rectangle minbounds = getBounds();
			minbounds.x = 0;
			minbounds.y = 0;
			if (mouseEvent->type == MOUSE_EVENT_RELEASE && minbounds.contains(mouseEvent->position)) 
			{
				checked = !checked;
			}

			markFor(COMPONENT_REQUIREMENT_PAINT);
		}
		return true;
	}

	return false;
}

/**
 *
 */
void Checkbox_t::handleBoundChange(Rectangle oldBounds) 
{
	Rectangle unpositioned = getBounds();
	unpositioned.x = boxSize + boxTextGap;
	unpositioned.y = 0;
	this->label.setBounds(unpositioned);
}

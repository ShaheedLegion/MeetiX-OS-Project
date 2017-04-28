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

#ifndef __FOCUS_EVENT__
#define __FOCUS_EVENT__

#include <events/event.hpp>
#include <stdint.h>

/**
 *
 */
enum FocusEventType_t 
{
	FOCUS_EVENT_NONE, 
	FOCUS_EVENT_GAINED, 
	FOCUS_EVENT_LOST
};

/**
 *
 */
class FocusEvent_t: public Event_t 
{
public:
	FocusEvent_t() : type(FOCUS_EVENT_NONE), newFocusedComponent(nullptr) 
	{
	}

	FocusEventType_t type;
	Component_t *newFocusedComponent;
};

#endif

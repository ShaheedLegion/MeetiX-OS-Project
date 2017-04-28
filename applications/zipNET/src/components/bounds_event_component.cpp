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

#include <components/bounds_event_component.hpp>
#include <components/component.hpp>
#include <stdio.h>

/**
 *
 */
void BoundsEventComponent_t::fireBoundsChange(Rectangle &bounds) 
{

	EventListenerInfo_t listenerInfo;
	if (self->getListener(UI_COMPONENT_EVENT_TYPE_BOUNDS, listenerInfo)) 
	{
		UiComponentBoundsEvent boundsEvent;
		boundsEvent.header.type = UI_COMPONENT_EVENT_TYPE_BOUNDS;
		boundsEvent.header.componentID = listenerInfo.componentID;
		boundsEvent.bounds = bounds;
		SendMessage(listenerInfo.targetThread, &boundsEvent, sizeof(UiComponentBoundsEvent));
	}
}

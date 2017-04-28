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

#include <components/action_component.hpp>
#include <components/component.hpp>
#include <stdio.h>

/**
 *
 */
void ActionComponent_t::fireAction() 
{
	// if there is an internal handler, use it
	if (internalHandler) 
	{
		internalHandler->handle(this);
		return;
	}

	// otherwise send message to registered thread
	EventListenerInfo_t listenerInfo;
	if (self->getListener(UI_COMPONENT_EVENT_TYPE_ACTION, listenerInfo)) 
	{
		UiComponentActionEvent actionEvent;
		actionEvent.header.type = UI_COMPONENT_EVENT_TYPE_ACTION;
		actionEvent.header.componentID = listenerInfo.componentID;
		SendMessage(listenerInfo.targetThread, &actionEvent, sizeof(UiComponentActionEvent));
	}
}

/**
 *
 */
void ActionComponent_t::setInternalActionHandler(InternalActionHandler_t *handler) 
{
	this->internalHandler = handler;
}

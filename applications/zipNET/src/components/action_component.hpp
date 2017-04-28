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

#ifndef __ACTION_COMPONENT__
#define __ACTION_COMPONENT__

#include <eva.h>
#include <string>
#include <list>
#include <mxuser/ui/interface_specification.hpp>

class Component_t;
class ActionComponent_t;

/**
 * Used so the window server can itself be the handler for an action component.
 */
class InternalActionHandler_t 
{
public:
	virtual ~InternalActionHandler_t() {}
	virtual void handle(ActionComponent_t *source) = 0;
};

/**
 * An action component is capable of being observed by an action listener.
 * The component may fire actions which are dispatched to the registered
 * listener for processing.
 */
class ActionComponent_t 
{
protected:
	Component_t *self;
	InternalActionHandler_t *internalHandler;

public:
	/**
	 *
	 */
	ActionComponent_t(Component_t *self) : self(self), internalHandler(nullptr) {}

	/**
	 *
	 */
	virtual ~ActionComponent_t() {}

	/**
	 *
	 */
	virtual void fireAction();

	/**
	 *
	 */
	virtual void setInternalActionHandler(InternalActionHandler_t *handler);

};

#endif

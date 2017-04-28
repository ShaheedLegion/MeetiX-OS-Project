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

#ifndef __BOUNDS_EVENT_COMPONENT__
#define __BOUNDS_EVENT_COMPONENT__

#include <eva.h>
#include <string>
#include <list>
#include <mxuser/ui/interface_specification.hpp>

class Component_t;

/**
 *
 */
class BoundsEventComponent_t 
{
private:
	Component_t *self;

public:
	/**
	 *
	 */
	BoundsEventComponent_t(Component_t *self) : self(self) 
	{
	}

	/**
	 *
	 */
	virtual ~BoundsEventComponent_t() 
	{
	}

	/**
	 *
	 */
	virtual void fireBoundsChange(Rectangle &bounds);
};

#endif

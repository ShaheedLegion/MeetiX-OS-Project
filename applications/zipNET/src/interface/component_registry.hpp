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

#ifndef __COMPONENT_REGISTRY__
#define __COMPONENT_REGISTRY__

#include <mxuser/tasking/thread.hpp>
#include <components/component.hpp>
#include <components/window.hpp>
#include <mxuser/ui/interface_specification.hpp>
#include <list>

using namespace std;

class Window_t;

/**
 *
 */
class ComponentRegistry_t 
{
public:
	static UiComponentID add(Pid process, Component_t *component);
	static Component_t *get(UiComponentID id);
	static std::list<Window_t*> getWindowsComponents();
	static map<UiComponentID, Component_t*> *getProcessMap(Pid pid);

	static bool removeComponent(Pid pid, UiComponentID id);
	static void removeProcessMap(Pid pid);
};

#endif

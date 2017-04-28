/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <interface/task_manager_thread.hpp>
#include <interface/component_registry.hpp>
#include <sstream>
#include <list>

/*
 *
 */
TaskManagerThread_t::TaskManagerThread_t(Label_t *where, const Rectangle &bounds) : internal(where), bounds(bounds) {}

/*
 *
 */
void TaskManagerThread_t::run()
{
	TaskRegisterID("zipNET/TaskManagerThread");

	internal->setVisible(true);
	internal->setColor(0, RGB(255, 255, 255));

	while (true)
	{
		list<Window_t*> windows = ComponentRegistry_t::getWindowsComponents();
		stringstream titles;

		for (Window_t *current : windows)
			titles << " | " << current->getTitle() << " | ";

		internal->setTitle(titles.str());
		Sleep(500);
	}
}
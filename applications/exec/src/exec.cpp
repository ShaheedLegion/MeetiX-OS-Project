/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHout ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <eva.h>
#include <sstream>

#include <mxuser/utils/environment.hpp>
#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/action_listener.hpp>
#include <mxuser/ui/MsgBox.hpp>

using namespace std;

bool buttonPressed = false;

/*
 *	this struct contains all components
 */
struct ComponentsOnScreen
{
	// main window
	Window *window;

	// show memory consuption
	Label *memConsupt;

	// textfield where write command
	Textfield *exec;

	// exec command
	Button *launch;

	// constructor instantiate all components
	ComponentsOnScreen() : window(Window::create()), memConsupt(Label::create()), exec(Textfield::create()), launch(Button::create()) {}

	// destructor deallocate all components
	~ComponentsOnScreen() 
	{ 
		memConsupt->remove();
		exec->remove();
		launch->remove();
		window->remove();
	}
};

/*
 *	action listener of button launch
 */
class LaunchListener : public ActionListener
{
public:
	virtual void handleAction() { buttonPressed = true; }
};

/*
 * use label to show memory consuption
 */
static void totalMemoryUsage(Label *label)
{
	while (true)
	{
		// get info from kernel
		SyscallSysinfo usage;// = Sysinfo();
		stringstream out;

		// process information
		int width = (usage.totRam / 1024) / 10;
		int used = (usage.totRam / 1024) - (usage.freeRam / 1024);
		int percent = used / 100 * width;
		int cells = (width * percent) / 100;

		// store info on strinstream
		out << "Total Memory Usage [";		
		for (int index = 0; index < width; index++)
		{
			if (index <= cells)	out << '|';
			else out << ' ';
		}
		out << ']' << used << '/' << usage.totRam / 1024 << "MB";

		// set on label
		label->setTitle(out.str());
	}
}

/*
 *	main of application
 */
int main(int argc, char *argv[])
{
	// ask windowserver zipNET to open a comunication channel
	UiOpenStatus openStat = UI::open();

	// if channel is open 
	if (openStat == UI_OPEN_STATUS_SUCCESSFUL) 
	{
		// create components
		ComponentsOnScreen components;

		// configuring window
		components.window->setBounds(Rectangle(100, 100, 250, 150));
		components.window->setColor(RGB(200, 0, 0), RGB(255, 255, 255));
		components.window->setTitle("Exec");

		// configuring label
		components.memConsupt->setBounds(Rectangle(0, 20, 250, 30));
		components.window->addChild(components.memConsupt);

		// configuring textfield
		components.exec->setBounds(Rectangle(0, 70, 250, 30));
		components.exec->setTitleAlignment(TextAlignment::LEFT);
		components.exec->setGhostTitle("Enter app name");
		components.window->addChild(components.exec);

		// configuring button
		components.launch->setBounds(Rectangle(65, 110, 120, 30));
		components.launch->setTitle("Esegui");
		components.launch->setActionListener(new LaunchListener());
		components.window->addChild(components.launch);

		// show window
		components.window->setVisible(true);

		// run thread
		CreateThreadD((void*) totalMemoryUsage, &components.memConsupt);

		// iterate
		while (true)
		{
			// wait 500
			Sleep(500);

			// get command
			if (buttonPressed)
			{
				string command = components.exec->getTitle();
				if (!command.empty())
				{
					Spawn(command.c_str(), "", "/", SECURITY_LEVEL_APPLICATION);
					buttonPressed = false;
				}
			}
		}
	}
}
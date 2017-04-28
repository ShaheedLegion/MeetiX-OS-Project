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

#include "SecondaryThread.hpp"
#include <mxuser/ui/button_list.hpp>
#include <mxuser/ui/component_registry.hpp>
#include <mxuser/utils/environment.hpp>

using namespace std;

/**
 *	Display hour to hour label
 */
void SecondaryThread::HourManagerThread(Label *hourLabel)
{
	// registering task id
	TaskRegisterID("CandyGUI/HourManagerThread");
	
	// creating object to time struct
	TimeDriverCall *time;

	// infinite iteration
	while (true)
	{
		// fill time struct
		time = MXtime::getTime();

		// write data to stringstream
		stringstream hourStr;
		hourStr << time->hour << ':' << time->minute << ':' << time->second;

		// and display on screen label
		hourLabel->setTitle(hourStr.str());

		// delete pointer
		delete time;

		// wait 1 second
		Sleep(1000);

	}
}

/**
 *	display memory usage in CandyGUI menu
 */
void SecondaryThread::MemoryUsageThread(Label *memLabel)
{
	// registering task id
	TaskRegisterID("CandyGUI/MemoryUsageThread");

	// infinite iteration
	while (true)
	{ 
		// stringstream convertor
		stringstream usage;
		
		// call sysinfo 
		SyscallSysinfo memUsage = Sysinfo();

		// fill stringstream with data
		usage << (memUsage.totRam - memUsage.freeRam) / 1024 << "/"  << memUsage.totRam / 1024  << "MB";

		// and display in screen label
		memLabel->setTitle(usage.str());

		// wait 50 milliseconds
		Sleep(50);
	}
}

/*
 * this method is called by button reboot
 */
void shutBoxMessage()
{
	// creating list for buttons
	ButtonList bnts;

	Dimension resolution = UI::getResolution();

	// create and set geoshape form for dialog
	Geoshape *dialog = Geoshape::create();
	dialog->setBounds(Rectangle(resolution.width / 2 - 240, resolution.height / 2 - 125, 480, 250));
	dialog->setTitle("Reboot or Shutdown?");
	dialog->setColor(ARGB(180, 100, 100, 100), ARGB(255, 0, 0, 0));

	// create and set label for message
	Label *lbl = Label::create();
	lbl->setBounds(Rectangle(50, 85, 400, 30));
	lbl->setFont("consolas");
	lbl->setTitle(Environment::getLoggedUser() + " are you sure you want to quit MeetiX OS?");
	dialog->addChild(lbl);

	// add button reboot to list
	bnts.add("reboot", []{ RebootSystem(); });
	bnts.configure("reboot", Rectangle(30, 190, 100, 30), "Reboot", ARGB(255, 0, 130, 0), ARGB(255, 0, 0, 0));

	// add button shutdown to list
	bnts.add("shutdown", []{ ShutdownSystem(); });
	bnts.configure("shutdown", Rectangle(340, 190, 100, 30), "Shutdown", ARGB(255, 130, 0, 0), ARGB(255, 0, 0, 0));

	// show buttons to dialog
	bnts.show(dialog);

	// show dialog
	dialog->setVisible(true);
}

/**
 *	Paint dock on screen
 */
void SecondaryThread::CandyDockThread(const Dimension &resolution)
{
	// register task name
	TaskRegisterID("CandyGUI/CandyDock");

	// creating button list
	ButtonList dockButtons;

	// create dock geoshape
	Geoshape *dock = Geoshape::create();
	dock->setBounds(Rectangle(resolution.width / 2 - 197, resolution.height - 64, 394, 64));
	dock->setColor(ARGB(120, 255, 255, 255), ARGB(255, 0, 0, 0));

	// creating button and add to list
	dockButtons.add("shell", []{ Spawn("/bin/CandyShell", "", "/", SECURITY_LEVEL_APPLICATION); });
	dockButtons.configure("shell", Rectangle(30, 5, 48, 48), "", "/usr/share/icons/term1.png", Point(0, 0), ARGB(0, 0, 0, 0), ARGB(255, 0, 0, 0));

	// creating button and add to list
	dockButtons.add("calculator", []{ Spawn("/bin/calculator", "", "/", SECURITY_LEVEL_APPLICATION); });
	dockButtons.configure("calculator", Rectangle(125, 5, 48, 48), "", "/usr/share/icons/calc1.png", Point(0, 0), ARGB(0, 0, 0, 0), ARGB(255, 0, 0, 0));

	// creating button and add to list
	dockButtons.add("editor", []{ Spawn("/bin/CandyNote", "", "/", SECURITY_LEVEL_APPLICATION); });
	dockButtons.configure("editor", Rectangle(220, 5, 48, 48), "", "/usr/share/icons/med1.png", Point(0, 0), ARGB(0, 0, 0, 0), ARGB(255, 0, 0, 0));
	
	// creating button and add to list
	dockButtons.add("shutdowns", []{ shutBoxMessage(); });
	dockButtons.configure("shutdowns", Rectangle(315, 5, 48, 48), "", "/usr/share/icons/shut1.png", Point(0, 0), ARGB(0, 0, 0, 0), ARGB(255, 0, 0, 0));
	
	// add all buttons to dock
	dockButtons.show(dock);

	// lock thread
	uint8_t blocker = true;
	AtomicBlock(&blocker);
}
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

#ifndef _CandyGui_
#define _CandyGui_

#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/MsgBox.hpp>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/geoshape.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/action_listener.hpp>
#include <mxuser/ui/button_list.hpp>
#include <mxuser/ui/key_listener.hpp>
#include <mxuser/graphics/metrics/dimension.hpp>
#include <mxuser/tasking/lock.hpp>
#include <math.h>
#include <map>

#include <SecondaryThread.hpp>

/*
 *
 */
class CandyGUI
{
public:
	CandyGUI() {}
	~CandyGUI();

	// initialize UI
	bool init();
	
	// configure ui with provided script
	void configureUi(std::string pathToConfiguration, Dimension resolution);
	
	// create the components of ui
	void createComponents();
	
	// main loop of ui
	void mainLoop();

private:
	enum UiStyle
	{
		GNOME,
		KDE,
	};

	// setup MeetiX menu button
	void setMenuButton();

	// setup menu tab
	void setMenuTab();
	
	// setup memory usage label
	void setMemLabel();

	// setup hour label
	void setHourLabel();
	
	// set task label
	void setTaskLabel();

	// create app button
	void createMenu();

	// read from keyboard
	KeyInfo readInput();

	// main bar of interface
	Geoshape *Bar;
	Button *about;

	// resolution of screen
	Dimension resolution;

	// container for all buttons on interface
	ButtonList buttons;

	// map that contains configuration provided by .candygui file
	std::map<std::string, std::string> configuration;
	UiStyle mode;
};

#endif

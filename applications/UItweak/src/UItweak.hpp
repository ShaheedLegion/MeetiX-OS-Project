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

#ifndef _UI_TWEAK_
#define _UI_TWEAK_

#include <eva.h>
#include <fstream>

#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/action_listener.hpp>

using namespace std;

/*
 *
 */
class UItweak
{
public:
	UItweak() {}
	
	//initialize UITweak if ui is open
	bool init();

	//paint all components of window
	void paintComponents();

	//function for ButtonListener
	static void ThemeConfig();
	
private:

	//components
	void createUItweakWindow();
	void colorTweaker();
	void backgroundTweaker();
	void buttonTweaker();

	//button of exit and apply
	void createApplyButton();
	void createExitButton();

	//object variables for components
	Window *window;
	
	Label *winColorLabel;

	Label *backgroundLabel;

	Label *buttonLabel;

	Button *applyButton;

	Button *exit;
};

#endif
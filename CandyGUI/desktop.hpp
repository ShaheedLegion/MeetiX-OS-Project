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

#include <ghostuser/ui/ui.hpp>
#include <ghostuser/ui/window.hpp>
#include <ghostuser/ui/label.hpp>
#include <ghostuser/ui/canvas.hpp>
#include <ghostuser/ui/button.hpp>
#include <ghostuser/ui/textfield.hpp>
#include <ghostuser/ui/action_listener.hpp>
#include <ghostuser/tasking/lock.hpp>
#include <math.h>

#include <SecondaryThread.hpp>

class CandyGUI_t
{
public:
	CandyGUI_t()
	{
	}

	//initialize UI
	bool init();
	
	//initialize canvas buffer
	void initCanvas();
	
	//create the components of ui
	void createComponents();
	
	//main loop of ui
	void mainLoop();

private:
	//setup MeetiX menu button
	void setMenuButton();

	//setup menu tab
	void setMenuTab();

	//setup hour label
	void setHourLabel();

	//create app button
	void appButton();

	//set task label
	void setTaskLabel();


	//variables
	g_canvas *canvas;
	cairo_surface_t *bufferSurface;
	cairo_t *cr;

	cairo_surface_t *surface;
	g_button *menuButton;
};

#endif

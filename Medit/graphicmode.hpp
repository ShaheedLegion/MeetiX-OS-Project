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

#ifndef _MediT_GraphicMode_H_
#define _MediT_GraphicMode_H_

#include <cairo/cairo.h>
#include <ghostuser/ui/window.hpp>
#include <ghostuser/ui/canvas.hpp>

#include <ghostuser/utils/utils.hpp>
#include <ghostuser/ui/ui.hpp>
#include <ghostuser/ui/label.hpp>
#include <ghostuser/ui/button.hpp>
#include <ghostuser/ui/textfield.hpp>
#include <ghostuser/ui/action_listener.hpp>
#include <ghostuser/ui/focus_listener.hpp>
#include <ghostuser/ui/key_listener.hpp>
#include <ghostuser/tasking/lock.hpp>
#include <ghostuser/graphics/text/font_loader.hpp>
#include <ghostuser/graphics/text/font.hpp>
#include <ghostuser/graphics/text/text_layouter.hpp>

class GraphicMode_t
{
public:
	
	/* constructor */
	GraphicMode_t()
	{

	}

	/*main of graphic mode*/
	void GraphicMode();

	/*set backend of application*/
	void initialize();

	/*if initialization fail*/
	void GraphicFail();

	/*crete windows and set it*/
	void createAmbient();

	/*create task bar*/
	void createTaskBar(FILE *file);

	/*create text field*/
	void paintTextSpace();

	std::stringstream text;
	std::string line;

private:

	/*cursor thread and research graphic buffer*/
	static void blinckCursorThread();
	cairo_t *getGraphics();

	void readInput();
	
	/*mask of Medit*/
	g_window *MeditWindow;
	
	/*font controller*/
	g_font *font;
	g_layouted_text* viewModel;

	/*taskbar buttons*/
	g_button *FileButton;
	g_button *HelpButton;

	/*text space*/
	g_canvas *textField;

	/*cairo's buffer variables*/
	cairo_surface_t* existingSurface = 0;
	g_color_argb* existingSurfaceBuffer = 0;
	g_dimension bufferSize;
	cairo_t* existingContext;
};

#endif
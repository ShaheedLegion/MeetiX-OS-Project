/**********************************************************************************
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
**********************************************************************************/

#include "graphicmode.hpp"
#include <Medit.hpp>

#include <ghost.h>
#include <ghostuser/utils/utils.hpp>
#include <ghostuser/io/keyboard.hpp>

uint8_t paintIsFresh 	= false;
bool cursorBlink 		= false;
bool focused 			= false;
uint64_t lastInput 		= 0;
std::string output;

/**
 *
 */
class FileButtonListener_t : public g_action_listener, GraphicMode_t
{
public:

	FileButtonListener_t(FILE *file)
	{
		save = file;
	}

	virtual void handle_action()
	{
		std::string textstr = text.str();
		const char *textStream = textstr.c_str();

		fwrite(textStream, sizeof(char), strlen(textStream), save);
	}

private:
	FILE *save;
};

/**
 *
 */
class HelpButtonListener_t : public g_action_listener
{
public:

	virtual void handle_action()
	{
		g_window *help = g_window::create();
		help->setTitle("Medit Help");

		g_rectangle helpBounds(500, 500, 500, 150);

		help->setBounds(helpBounds);
		help->setVisible(true);
		help->setResizable(false);

		g_label *helpLabel = g_label::create();
		helpLabel->setTitle(helpstr.c_str());
		helpLabel->setBounds(g_rectangle(0, 0, helpBounds.width, 30));

		help->addChild(helpLabel);
	}

private:
	std::string helpstr = "hello, this is Graphic Medit: edit your text and save it with save button";
};

/**
 *
 */
class canvas_resize_bounds_listener_t: public g_bounds_listener 
{
public:
	g_canvas* canvas;

	canvas_resize_bounds_listener_t(g_canvas* canvas) : canvas(canvas) 
	{

	}

	virtual void handle_bounds_changed(g_rectangle bounds) 
	{
		paintIsFresh = false;
	}
};

/**
 *
 */
void GraphicMode_t::GraphicMode()
{
	initialize();
	createAmbient();

	FILE *file = fopen("newfile.txt", "w+");
	createTaskBar(file);

	while (true)
	{
		paintTextSpace();

		readInput();
	}
}

/**
 *
 */
void GraphicMode_t::initialize()
{
	if (!g_task_register_id("Medit"))
	{
		GraphicFail();
		g_exit(1);
	}

	// set working directory
	g_set_working_directory(Medit_dir);

	// load keyboard layout
	g_keyboard::loadLayout("it-EU");
}

/**
 *
 */
void GraphicMode_t::GraphicFail()
{
	g_window *WindowError = g_window::create();
	WindowError->setTitle("Error");

	g_label *lberr = g_label::create();
	lberr->setBounds(g_rectangle(20, 15, 400, 15));
	lberr->setTitle("Unable to run MediT Editor with this identifier");
	WindowError->addChild(lberr);
	
	g_rectangle errWin(800, 600, 500, 120);
	WindowError->setBounds(errWin);
	WindowError->setVisible(true);
}

/**
 *
 */
void GraphicMode_t::createAmbient()
{
	MeditWindow = g_window::create();
	MeditWindow->setTitle("Medit Editor");

	MeditWindow->setBoundsListener(new canvas_resize_bounds_listener_t(textField));

	textField = g_canvas::create();
	MeditWindow->addChild(textField);

	g_rectangle MeditWindowBounds(20, 20, 600, 600);
	MeditWindow->setBounds(MeditWindowBounds);
	textField->setBounds(g_rectangle(0, 0, MeditWindowBounds.width, MeditWindowBounds.height));
	MeditWindow->setVisible(true);

	font = g_font_loader::get("consolas");
	viewModel = g_text_layouter::getInstance()->initializeBuffer();
}

void GraphicMode_t::createTaskBar(FILE *file)
{
	FileButton = g_button::create();
	FileButton->setTitle("Save");
	FileButton->setBounds(g_rectangle(0, 0, 45, 30));
	MeditWindow->addChild(FileButton);
	FileButton->setActionListener(new FileButtonListener_t(file));

	HelpButton = g_button::create();
	HelpButton->setTitle("Help");
	HelpButton->setBounds(g_rectangle(45, 0, 45, 30));
	MeditWindow->addChild(HelpButton);
	HelpButton->setActionListener(new HelpButtonListener_t());
}

/**
 *
 */
void GraphicMode_t::blinckCursorThread()
{
	while (true) 
	{
		cursorBlink = !cursorBlink;
		paintIsFresh = false;
		g_sleep(650);
	}
}

/**
 *
 */
cairo_t* GraphicMode_t::getGraphics()
{

	// get buffer
	auto bufferInfo = textField->getBuffer();
	if (bufferInfo.buffer == 0) 
	{
		return 0;
	}

	bufferSize.width = bufferInfo.width;
	bufferSize.height = bufferInfo.height;

	// get the surface ready and go:
	if (existingSurface == 0 || existingSurfaceBuffer != bufferInfo.buffer) 
	{
		if (existingContext != 0) 
		{
			cairo_destroy(existingContext);
		}

		existingSurface = cairo_image_surface_create_for_data(
				(uint8_t*) bufferInfo.buffer, CAIRO_FORMAT_ARGB32,
				bufferInfo.width, bufferInfo.height,
				cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32,
						bufferInfo.width));
		existingSurfaceBuffer = bufferInfo.buffer;
		existingContext = cairo_create(existingSurface);
	}

	return existingContext;
}

/**
 *
 */
void GraphicMode_t::paintTextSpace()
{
	g_create_thread((void*) blinckCursorThread);

	int padding = 5;
	while (true) 
	{
		auto windowBounds = MeditWindow->getBounds();
		textField->setBounds(g_rectangle(0, 30, windowBounds.width, windowBounds.height));

		auto cr = getGraphics();
		if (cr == 0) 
		{
			g_sleep(100);
			continue;
		}

		// clear
		cairo_save(cr);
		cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);

		// relayout text
		g_text_layouter::getInstance()->layout(cr, output.c_str(), font, 14,
				g_rectangle(padding, padding,
						windowBounds.width - 2 * padding - 20,
						windowBounds.height - 2 * padding),
				g_text_alignment::LEFT, viewModel, true);

		// check which is the lowest-down-the-screen character
		int highesty = 0;
		for (g_positioned_glyph& g : viewModel->positions) 
		{
			int ypos = g.position.y - g.glyph->y;
			if (ypos > highesty) 
			{
				highesty = ypos;
			}
		}

		// calculate limit
		int yLimit = windowBounds.height - 60;
		int yOffset = 0;
		if (highesty > yLimit) 
		{
			yOffset = yLimit - highesty;
		}

		// paint characters
		g_point last;
		cairo_set_source_rgba(cr, 0.5, 0.5, 0.5, 1);
		for (g_positioned_glyph& g : viewModel->positions) 
		{
			last = g.position;

			cairo_save(cr);
			cairo_translate(cr, g.position.x - g.glyph->x, yOffset + g.position.y - g.glyph->y);
			cairo_glyph_path(cr, g.glyph, g.glyph_count);
			cairo_fill(cr);
			cairo_restore(cr);
		}

		// paint cursor
		if (focused) 
		{
			if ((g_millis() - lastInput < 300) || cursorBlink) 
			{
				cairo_save(cr);
				cairo_set_source_rgba(cr, 0.8, 0.8, 0.8, 1);
				cairo_rectangle(cr, last.x + 10, yOffset + last.y - 12, 8, 14);
				cairo_fill(cr);
				cairo_restore(cr);
			}
		}

		textField->blit(g_rectangle(0, 0, bufferSize.width, bufferSize.height));

		paintIsFresh = true;
		g_atomic_block(&paintIsFresh);
	}
}

void GraphicMode_t::readInput()
{

}
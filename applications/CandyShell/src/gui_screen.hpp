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

#include "color.hpp"
#include "screen.hpp"
#include "candyshell.hpp"

#include <cairo/cairo.h>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/canvas.hpp>

#include <mxuser/utils/utils.hpp>
#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/action_listener.hpp>
#include <mxuser/ui/focus_listener.hpp>
#include <mxuser/ui/key_listener.hpp>
#include <mxuser/tasking/lock.hpp>
#include <mxuser/graphics/text/font_loader.hpp>
#include <mxuser/graphics/text/font.hpp>
#include <mxuser/graphics/text/text_layouter.hpp>

using namespace std;

/**
 *
 */
class GuiScreen : public Screen 
{
private:
	Window *window;
	Canvas *canvas;

	cairo_surface_t *existingSurface = 0;
	ColorArgb *existingSurfaceBuffer = 0;
	Dimension bufferSize;
	cairo_t *existingContext;

	LayoutedText *viewModel;
	Font_t *font;

	ColorArgb backgroundColor = ARGB(0, 0, 0, 0);
	ColorArgb fontColor = RGB(255, 255, 255);

	void initialize();
	cairo_t *getGraphics();

	static void blinkCursorThread();

public:
	GuiScreen();
	~GuiScreen();

	static void paintEntry();
	void paint();

	void clean();
	void deactivate();
	void activate();

	void close();

	bool setColor(string color);

	void backspace();
	void cleanLine(int lineLenght);
	void write(string message, ScreenColor color = Color::color(SC_STD_BACK, SC_LGRAY), bool visible = true);
	void writeChar(char c, ScreenColor color = Color::color(SC_STD_BACK, SC_LGRAY));
	void updateCursor();

	KeyInfo readInput(bool *cancelCondition);

	void workingDirectoryChanged(string str);
};

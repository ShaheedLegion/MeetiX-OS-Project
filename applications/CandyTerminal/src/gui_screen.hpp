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

#include "screen.hpp"

#include <map>
#include <list>
#include <cairo/cairo.h>

#include <mxuser/ui/window.hpp>
#include <mxuser/ui/canvas.hpp>
#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/action_listener.hpp>
#include <mxuser/ui/focus_listener.hpp>
#include <mxuser/ui/key_listener.hpp>
#include <mxuser/utils/utils.hpp>
#include <mxuser/tasking/lock.hpp>
#include <mxuser/graphics/text/font_loader.hpp>
#include <mxuser/graphics/text/font.hpp>
#include <mxuser/graphics/text/text_layouter.hpp>

/**
 * Used to cache information about the layout of a specific character.
 */
struct CharLayout 
{
	cairo_glyph_t *glyphBuffer = nullptr;
	int glyphCount;
	cairo_text_cluster_t *clusterBuffer = nullptr;
	int clusterCount;
};

/**
 *
 */
class GuiScreen : public Screen 
{
private:
	/**
	 * UI related properties
	 */
	Window *window;
	Canvas *canvas;

	Font_t *font;

	cairo_surface_t *existingSurface = 0;
	ColorArgb *existingSurfaceBuffer = 0;
	Dimension bufferSize;
	cairo_t *existingContext = 0;

	uint8_t paintUpToDate = false;
	bool cursorBlink = false;

	std::list<KeyInfo> inputBuffer;
	uint8_t inputBufferEmpty = true;
	Lock inputBufferLock;

	bool focused = false;
	uint64_t lastInputTime = 0;

	/**
	 * Screen buffer
	 */
	uint8_t *rasterBuffer = 0;
	Dimension rasterSize;
	Lock rasterLock;
	int cursorX = 0;
	int cursorY = 0;

	int charWidth = 8;
	int charHeight = 12;

	std::map<char, CharLayout*> charLayoutCache;

	/**
	 * Prepares the canvas buffer for painting.
	 *
	 * @return the cairo instance to paint with
	 */
	cairo_t *getGraphics();

	/**
	 * Thread that is responsible for making the cursor
	 * blink in specific intervals.
	 *
	 * @param screen
	 * 		instance of the screen
	 */
	static void blinkCursorEntry(GuiScreen *screen);
	void blinkCursor();

	/**
	 * paint routine
	 */
	static void paintEntry(GuiScreen *screen);
	void paint();

	/**
	 *
	 */
	CharLayout* getCharLayout(cairo_scaled_font_t *scaledFace, char c);

public:
	/**
	 * Initializes the UI components for the screen.
	 *
	 * @return whether initialization was successful
	 */
	bool initialize();

	KeyInfo readInput();
	void clean();
	void writeChar(char c);
	void moveCursor(int x, int y);
	int getCursorX();
	int getCursorY();
	void backspace();

	void bufferInput(const KeyInfo &info);
	void repaint();
	void setFocused(bool focused);

	void updateVisibleBufferSize();
};

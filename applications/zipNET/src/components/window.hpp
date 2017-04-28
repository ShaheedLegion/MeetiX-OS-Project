/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef __WINDOW__
#define __WINDOW__

#include <components/component.hpp>
#include <components/label.hpp>
#include <components/titled_component.hpp>
#include <components/png_component.hpp>
#include <components/colored_component.hpp>
#include <components/panel.hpp>

#include <mxuser/ui/properties.hpp>
#include <mxuser/ui/interface_specification.hpp>

/**
 * constants for border sizes
 */
#define DEFAULT_BORDER_WIDTH		7
#define DEFAULT_CORNER_SIZE			15

/**
 * modes used when resizing windows
 */
enum WindowResizeMode
{
	RESIZE_MODE_NONE,
	RESIZE_MODE_MOVE,
	RESIZE_MODE_TOP,
	RESIZE_MODE_TOP_RIGHT,
	RESIZE_MODE_RIGHT,
	RESIZE_MODE_BOTTOM_RIGHT,
	RESIZE_MODE_BOTTOM,
	RESIZE_MODE_BOTTOM_LEFT,
	RESIZE_MODE_LEFT,
	RESIZE_MODE_TOP_LEFT
};

/**
 *
 */
class Window_t: public Component_t, public TitledComponent_t, public PngComponent_t, public ColoredComponent_t
{
private:
	int borderWidth;
	int cornerSize;
	bool resizable;

	cairo_t *cr;
	Rectangle bounds;

	Label_t label;
	Panel_t panel;

	bool crossPressed;
	bool crossHovered;
	bool maximizeHovered;
	bool maximizePressed;
	bool isFullScreen;
	bool focused;

	int shadowSize;
	Rectangle SetInitialBounds;
	Rectangle crossBounds;
	Rectangle maximizeBounds;
	string style;

public:
	Window_t();

	virtual ~Window_t() { }

	/**
	 *
	 */
	Panel_t *getPanel() { return &panel; }

	/**
	 * component
	 */
	virtual void setFocus(bool focus);
	virtual void addChild(Component_t *component);
	virtual void layout();
	virtual void paint();

	/**
	 *	png component
	 */
	virtual void setPNG(std::string path, Point position);
	virtual void PngAnimation(std::string path, Point PNGstartAnimation, Point PNGendAnimation, size_t sleep);
	
	/**
	 * color component
	 */
	virtual void setColor(ColorArgb color, ColorArgb tltColor);

	/**
	 *
	 */
	virtual void handleBoundChange(Rectangle oldBounds);
	virtual bool handle(Event_t &e);
	virtual bool getNumericProperty(int property, uint32_t *out);
	virtual bool setNumericProperty(int property, uint32_t value);

	/**
	 *	titled component
	 */
	virtual void setTitle(std::string title);
	virtual std::string getTitle();
	virtual void setTitleFont(std::string fontName);
	virtual void setFontSize(int size) {}
	virtual void setTitleAlignment(TextAlignment alignment);

	/**
	 *	
	 */
	virtual void closeAll();
};

#endif

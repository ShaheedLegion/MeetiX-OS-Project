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

#ifndef __BUTTON__
#define __BUTTON__

#include <components/action_component.hpp>
#include <components/component.hpp>
#include <components/button_state.hpp>
#include <components/label.hpp>
#include <components/titled_component.hpp>
#include <components/png_component.hpp>
#include <components/colored_component.hpp>
#include <mxuser/graphics/text/font.hpp>
#include <mxuser/graphics/metrics/insets.hpp>
#include <string>

/**
 *
 */
class Button_t: public Component_t, public TitledComponent_t, public ActionComponent_t, public PngComponent_t, public ColoredComponent_t
{
private:
	ButtonState_t state;
	Label_t label;
	Insets insets;
	bool enabled;

	cairo_t *cr;
	Rectangle bounds;

	ColorArgb border;

public:
	/**
	 *
	 */
	Button_t();
	virtual ~Button_t() {}

	/*
	 * Component
	 */
	virtual void layout();
	virtual void paint();
	virtual bool handle(Event_t &e);
	Label_t &getLabel() { return label; }
	virtual void setFocus(bool focus);
	virtual void handleBoundChange(Rectangle oldBounds);
	virtual bool getNumericProperty(int property, uint32_t *out);
	virtual bool setNumericProperty(int property, uint32_t value);

	/*
	 *	titled component
	 */
	virtual void setTitle(std::string title);
	virtual std::string getTitle();
	virtual void setTitleFont(std::string fontName);
	virtual void setFontSize(int size);
	virtual void setTitleAlignment(TextAlignment alignment);

	/*
	 * png component
	 */
	virtual void setPNG(std::string path, Point position);
	virtual void PngAnimation(std::string path, Point PNGstartAnimation, Point PNGendAnimation, size_t sleep);
	
	/*
	 * colored component
	 */
	virtual void setColor(ColorArgb color, ColorArgb tltColor);
};

#endif

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

#ifndef __LABEL__
#define __LABEL__

#include <components/component.hpp>
#include <components/titled_component.hpp>
#include <components/colored_component.hpp>
#include <mxuser/graphics/text/font.hpp>
#include <mxuser/graphics/text/text_alignment.hpp>

/**
 *
 */
class Label_t : public Component_t, public TitledComponent_t, public ColoredComponent_t
{
private:
	Font_t *font;
	int fontSize;
	cairo_text_extents_t lastExtents;

	cairo_t *cr;
	Rectangle bounds;

	std::string text;
	TextAlignment alignment;

public:
	Label_t();
	virtual ~Label_t() { delete font; }

	/*
	 * Component
	 */
	virtual void paint();
	virtual void update();
	virtual bool handle(Event_t &e);

	/*
	 * titled component
	 */
	virtual void setTitle(std::string title);
	virtual std::string getTitle();
	virtual void setTitleFont(std::string fontName);
	virtual void setFontSize(int size);
	virtual void setTitleAlignment(TextAlignment alignment);

	/*
	 * colored component
	 */
	virtual void setColor(ColorArgb color, ColorArgb tltColor) { shapeColor = tltColor; }

	/*
	 * label
	 */
	virtual void setFont(Font_t *font);
	void setFontColor(ColorArgb ftcol);
	void setAlignment(TextAlignment alignment);
	TextAlignment getAlignment();
};

#endif

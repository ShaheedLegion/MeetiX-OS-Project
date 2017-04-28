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

#include <components/label.hpp>
#include <events/mouse_event.hpp>
#include <mxuser/utils/logger.hpp>

#include <cairo/cairo.h>
#include <cairo/cairo-ft.h>
#include <mxuser/graphics/text/font_loader.hpp>
#include <sstream>
#include <eva.h>

/**
 *
 */
Label_t::Label_t() : Component_t(true), alignment(TextAlignment::LEFT)
{
	type = UI_COMPONENT_TYPE_LABEL;
	setFont(FontLoader::getDefault());
	shapeColor = RGB(0, 0, 0);
}

/*
 *
 */
void Label_t::setFontSize(int size)
{
	fontSize = size;
}

/**
 *
 */
void Label_t::setFont(Font_t *newFont) 
{
	font = newFont;
	fontSize = 14;
}

/**
 *
 */
void Label_t::setTitleFont(std::string fontName)
{
	setFont(FontLoader::get(fontName));
	markFor(COMPONENT_REQUIREMENT_UPDATE);
}

/*
 *
 */
void Label_t::setTitleAlignment(TextAlignment alignment)
{
	this->alignment = alignment;
	markFor(COMPONENT_REQUIREMENT_ALL);
}

/**
 *
 */
void Label_t::update() 
{
	Rectangle thisBounds(0, 0, getBounds().width, getBounds().height);

	// Check if the component was ever layouted, otherwise set to a high value
	if (thisBounds.width == 0 && thisBounds.height == 0) 
	{
		thisBounds.width = 9999;
		thisBounds.height = 9999;
	}

	// get text bounds
	cr = graphics.getContext();
	cairo_set_font_face(cr, font->getFace());
	cairo_set_font_size(cr, fontSize);
	cairo_text_extents(cr, this->text.c_str(), &lastExtents);
	Dimension newPreferred(lastExtents.width + 3, lastExtents.height + 3);

	// Set new preferred size
	if (getPreferredSize() != newPreferred) setPreferredSize(newPreferred);
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
void Label_t::paint() 
{

	clearSurface();

	cr = graphics.getContext();
	bounds = getBounds();

	cairo_set_source_rgba(cr, ARGB_FR_FROM(shapeColor), ARGB_FG_FROM(shapeColor), ARGB_FB_FROM(shapeColor), ARGB_FA_FROM(shapeColor));

	int textLeft;
	int textBot = (bounds.height / 2 - lastExtents.height / 2) + lastExtents.height;

	if (alignment == TextAlignment::CENTER) textLeft = bounds.width / 2 - lastExtents.width / 2;

	else if (alignment == TextAlignment::RIGHT) textLeft = bounds.width - lastExtents.width;

	else textLeft = 0;

	cairo_move_to(cr, textLeft, textBot);
	cairo_set_font_face(cr, font->getFace());
	cairo_set_font_size(cr, fontSize);
	cairo_show_text(cr, text.c_str());
}

/**
 *
 */
bool Label_t::handle(Event_t &e) 
{
	return false;
}

/**
 *
 */
void Label_t::setTitle(std::string newText) 
{
	text = newText;
	markFor(COMPONENT_REQUIREMENT_UPDATE);
}

/**
 *
 */
std::string Label_t::getTitle() 
{
	return text;
}

/**
 *
 */
void Label_t::setFontColor(ColorArgb ftcol)
{
	setColor(0, ftcol);
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
void Label_t::setAlignment(TextAlignment newAlignment) 
{
	alignment = newAlignment;
	markFor(COMPONENT_REQUIREMENT_UPDATE);
}

/**
 *
 */
TextAlignment Label_t::getAlignment() 
{
	return alignment;
}

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

#ifndef __TEXT_FIELD__
#define __TEXT_FIELD__

#include <components/text/text_component.hpp>
#include <components/titled_component.hpp>
#include <components/colored_component.hpp>
#include <mxuser/graphics/metrics/insets.hpp>
#include <mxuser/graphics/text/text_layouter.hpp>
#include <mxuser/io/keyboard.hpp>

#include <string>
#include <list>

/**
 *
 */
enum class TextFieldVisualStatus_t : uint8_t 
{
	NORMAL, 
	HOVERED
};

/**
 *
 */
class TextField_t : public TextComponent_t, public TitledComponent_t, public ColoredComponent_t
{
private:
	std::string text;
	TextFieldVisualStatus_t visualStatus;
	TextAlignment alignment;
	bool focused;
	bool secure;

	Font_t *font;
	std::string ghostText;
	bool ghost;

	int scrollX;
	int fontSize;
	ColorArgb textColor;
	ColorArgb sColor;
	Insets insets;

	int cursor;
	int marker;

	LayoutedText *viewModel;

	cairo_t *cr;
	Rectangle bounds;

	void loadDefaultFont();
	void applyScroll();

public:
	TextField_t();
	virtual ~TextField_t();

	/**
	 * component
	 */
	virtual void update();
	virtual void paint();
	virtual bool handle(Event_t &e);
	virtual void setFocus(bool focus);
	virtual bool getNumericProperty(int property, uint32_t *out);
	virtual bool setNumericProperty(int property, uint32_t value);

	/**
	 * text component
	 */
	virtual void setCursor(int pos);
	virtual int getCursor() { return cursor; }
	virtual void setMarker(int pos);
	virtual int getMarker() { return marker; }
	virtual Range getSelectedRange();
	virtual void setGhostText(std::string text);
	virtual void setText(std::string text);
	virtual std::string getText() { return text; }

	/**
	 * titled component
	 */
	virtual void setTitle(std::string title) { setText(title); }
	virtual std::string getTitle() { return getText(); }
	virtual void setFont(Font_t *f);
	virtual void setTitleFont(std::string fontName);
	virtual void setFontSize(int size);
	virtual void setTitleAlignment(TextAlignment alignment);

	/**
	 * colored component
	 */
	virtual void setColor(ColorArgb ShapeColor, ColorArgb titleColor);

	/**
	 * textfield
	 */
	void backspace(KeyInfo &info);
	void insert(std::string text);
	int viewToPosition(Point p);
	Rectangle glyphToView(PositionedGlyph &g);
	int positionToUnscrolledCursorX(int pos);
	Rectangle positionToCursorBounds(int pos);
};

#endif

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

#ifndef __TEXT_COMPONENT__
#define __TEXT_COMPONENT__

#include <components/component.hpp>
#include <components/text/caret_direction.hpp>
#include <components/text/move/caret_move_strategy.hpp>
#include <mxuser/graphics/metrics/range.hpp>
#include <mxuser/graphics/text/font.hpp>

/**
 *
 */
class TextComponent_t : public Component_t 
{
protected:
	CaretMoveStrategy_t *caretMoveStrategy;

public:
	virtual ~TextComponent_t() {}

	/**
	 *
	 */
	virtual void setCursor(int position) = 0;

	/**
	 *
	 */
	virtual int getCursor() = 0;

	/**
	 *
	 */
	virtual void setMarker(int position) = 0;

	/**
	 *
	 */
	virtual int getMarker() = 0;

	/**
	 *
	 */
	virtual void setText(std::string text) = 0;

	/**
	 *
	 */
	virtual void setGhostText(std::string text) = 0;

	/**
	 *
	 */
	virtual std::string getText() = 0;

	/**
	 *
	 */
	virtual Range getSelectedRange() = 0;

	/**
	 *
	 */
	virtual void setFont(Font_t *font) = 0;
};

#endif

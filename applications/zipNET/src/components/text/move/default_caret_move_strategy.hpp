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

#ifndef __DEFAULT_CARET_MOVE_STRATEGY__
#define __DEFAULT_CARET_MOVE_STRATEGY__

#include <components/text/move/caret_move_strategy.hpp>

/*
 *
 */
class DefaultCaretMoveStrategy_t : public CaretMoveStrategy_t 
{
private:
	DefaultCaretMoveStrategy_t() {}

public:
	virtual ~DefaultCaretMoveStrategy_t() {}

	/**
	 *
	 */
	virtual void moveCaret(TextComponent_t *component, CaretDirection_t direction, KeyInfo &keyInfo);

	/**
	 *
	 */
	virtual int calculateSkip(std::string text, int position, CaretDirection_t direction);

	/**
	 *
	 */
	static DefaultCaretMoveStrategy_t *getInstance();
};

#endif

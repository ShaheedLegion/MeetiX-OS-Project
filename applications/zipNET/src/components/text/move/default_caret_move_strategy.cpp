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

#include <components/text/move/default_caret_move_strategy.hpp>
#include <components/text/text_component.hpp>

static DefaultCaretMoveStrategy_t *instance = 0;

/**
 *
 */
DefaultCaretMoveStrategy_t *DefaultCaretMoveStrategy_t::getInstance() 
{
	if (instance == 0) instance = new DefaultCaretMoveStrategy_t();
	return instance;
}

/**
 *
 */
void DefaultCaretMoveStrategy_t::moveCaret(TextComponent_t *component, CaretDirection_t direction, KeyInfo &info) 
{

	int cursor = component->getCursor();
	int newCursorPosition = cursor;
	int selectedLength = component->getSelectedRange().getLength();

	if (direction == CaretDirection_t::RIGHT) 
	{
		if (info.ctrl) newCursorPosition = component->getText().length();

		else newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, CaretDirection_t::RIGHT) : (cursor + 1);
	} 

	else if (direction == CaretDirection_t::LEFT) 
	{
		if (info.ctrl) newCursorPosition = 0;

		else newCursorPosition = info.alt ? calculateSkip(component->getText(), cursor, CaretDirection_t::LEFT) : (cursor - 1);
	}

	component->setCursor(newCursorPosition);
	if (!info.shift) component->setMarker(newCursorPosition);
}

/**
 *
 */
int DefaultCaretMoveStrategy_t::calculateSkip(std::string text, int position, CaretDirection_t direction) 
{

	bool l = (direction == CaretDirection_t::LEFT);

	if (l ? (position > 0) : (position < text.length())) 
	{
		bool inFirst = true;
		if (!l) 
		{
			char c = text[position];
			if (!(c == ' ' || c == ',' || c == '.')) 
			{
				inFirst = false;
			}
		}

		l ? --position : ++position;

		while (l ? position > 0 : position < text.length()) 
		{
			char c = text[position];
			char p = text[l ? position - 1 : position];

			if (inFirst && (c == ' ' || c == ',' || c == '.')) l ? --position : ++position;

			else if ((!l || !(p == ' ' || p == ',' || p == '.')) && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) 
			{
				l ? --position : ++position;
				inFirst = false;
			} 

			else break;
		}
	}

	return position;
}

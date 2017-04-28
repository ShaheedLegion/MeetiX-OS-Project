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

#ifndef _COLOR_HPP_
#define _COLOR_HPP_

#include <stdint.h>

/**
 *
 */
typedef uint8_t ScreenColor;

#define SC_BLACK	     0x0
#define SC_BLUE		     0x1
#define SC_GREEN	     0x2
#define SC_CYAN		     0x3
#define SC_RED		     0x4
#define SC_MAGENTA	     0x5
#define SC_BROWN	  	 0x6
#define SC_LGRAY	  	 0x7
#define SC_DARKGRAY	  	 0x8
#define SC_LBLUE	  	 0x9
#define SC_LGREEN	  	 0xA
#define SC_LCYAN	 	 0xB
#define SC_LRED		 	 0xC
#define SC_LMAGENTA	 	 0xD
#define SC_YELLOW	 	 0xE
#define SC_WHITE	 	 0xF

#define SC_STD_BACK	  	 0x0

class Color
{
public:

	Color();

	static ScreenColor color(ScreenColor ba, ScreenColor fo);
	ScreenColor defaultColor();
	ScreenColor errorColor();

	void SetColor(ScreenColor ba, ScreenColor fo, ScreenColor er = SC_RED);

private:
	ScreenColor stdBack;
	ScreenColor stdFont;
	ScreenColor stdTmp;
};

#endif
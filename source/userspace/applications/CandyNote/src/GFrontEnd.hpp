/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#ifndef _Graphic_FrontEnd_H_
#define _Graphic_FrontEnd_H_

#include <FrontEnd.hpp>

#include <list>
#include <fstream>

#include <cairo/cairo.h>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/canvas.hpp>

#include <mxuser/utils/utils.hpp>
#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/actionlistener.hpp>
#include <mxuser/ui/focuslistener.hpp>
#include <mxuser/ui/keylistener.hpp>
#include <mxuser/ui/msgbox.hpp>
#include <mxuser/tasking/lock.hpp>
#include <mxuser/graphics/text/fontldr.hpp>
#include <mxuser/graphics/text/font.hpp>
#include <mxuser/graphics/text/textlyot.hpp>

using namespace std;

class GraphicFrontEnd_t : public FrontEnd
{
public:
	// constructor
	GraphicFrontEnd_t();

	virtual void clean();

	virtual void end();

	virtual void write(string message, string title);

	virtual KeyInfo readInput();

private:
	void initialize();
	void paintEntry();
	void paintLoop();
};

#endif

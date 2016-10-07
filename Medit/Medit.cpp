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

#include "Medit.hpp"
#include <textMode/textmode.hpp>
#include <graphicMode/graphicmode.hpp>

#include <ghostuser/ui/ui.hpp>

int main(int argc, char *argv[])
{
	g_ui_open_status open_stat = g_ui::open();

	if (open_stat == G_UI_OPEN_STATUS_SUCCESSFUL) 
	{
		GraphicMode_t *grp = new GraphicMode_t();
		grp->GraphicMode();
	}

	else
	{
		TextMode_t *txt = new TextMode_t();
		txt->TextMode(argc, argv);
	}

	return 0;
}
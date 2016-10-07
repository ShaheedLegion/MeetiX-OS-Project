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

#include <desktop.hpp>

/**
 *
 */
int main(int argc, char** argv) 
{
	CandyGUI_t *ui = new CandyGUI_t();

	if (ui->init()) 
	{
		//initialize canvas
		ui->initCanvas();

		//create ui components
		ui->createComponents();

		//main loop
		ui->mainLoop();
	}

	else
	{ 
		klog("Unable to comunicate with zipNET");
		klog("Exit ");
		g_exit(-1);
	}

	return 0;
}

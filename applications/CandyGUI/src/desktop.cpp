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
#include <mxuser/io/files/file_log.hpp>

/**
 *	main of application
 */
int main(int argc, char *argv[]) 
{
	// creating object
	CandyGUI *ui = new CandyGUI();

	// initialize graphic comunication
	if (ui->init()) 
	{
		// get resolution from windowserver
		Dimension resolution = UI::getResolution();

		// configure ui with script
		ui->configureUi("/usr/share/cfg/.candygui", resolution);
		ui->createComponents();

		//main loop
		ui->mainLoop();
	}

	// print error message on log
	else FileLog::flog("CandyGui: Unable to comunicate with zipNET");

	// delete object
	delete ui;

	return 0;
}

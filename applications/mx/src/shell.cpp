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

#include <mx.hpp>

#include <iostream>
#include <mxuser/io/shell.hpp>
#include <stdio.h>

using namespace std;

/**
 *
 */
bool readInputLine(string &line) 
{

	Shell::setMode(SHELL_MODE_RAW);
	Shell::setEcho(false);

	int caret = 0;

	while (true) 
	{
		int c = Shell::getChar();
		if (c == -1) return false;

		if (c == SHELLKEY_BACKSPACE) 
		{
			if (line.size() > 0 && caret > 0) 
			{
				auto pos = Shell::getCursor();
				auto afterCaret = line.substr(caret);
				line = line.substr(0, caret - 1) + afterCaret;
				caret--;

				afterCaret += ' ';
				pos.x--;
				Shell::setCursor(pos);
				for (char c : afterCaret) 
				{
					Shell::putChar(c);
				}
				Shell::setCursor(pos);
			}

		} 

		else if (c == SHELLKEY_ENTER) 
		{
			Shell::putChar('\n');
			break;
		} 

		else if (c == SHELLKEY_LEFT) 
		{
			if (caret > 0) 
			{
				caret--;
				Shell::moveCursorBack(1);
			}
		} 

		else if (c == SHELLKEY_RIGHT) 
		{
			if (caret < line.size()) 
			{
				caret++;
				Shell::moveCursorForward(1);
			}
		} 

		else if (c < 0x100) 
		{
			auto pos = Shell::getCursor();
			Shell::putChar(c);

			auto afterCaret = line.substr(caret);
			line = line.substr(0, caret) + (char) c + afterCaret;
			caret++;

			pos.x++;
			Shell::setCursor(pos);
			for (char c : afterCaret) 
			{
				Shell::putChar(c);
			}
			Shell::setCursor(pos);
		}
	}

	return true;
}

/**
 *
 */
void MXShell::shellMode() 
{
	// copy to local variables main local variables
	string path = environment->getVariable("PATH");
	string user = environment->getVariable("USER");
	string hostname = environment->getVariable("HOSTNAME");

	Shell::setCursor(ShellCursorPosition(0, 0));

	char *dirBuff = new char[PATH_MAX];

	while (true) 
	{
		// print cwd
		if (GetWorkingDirectory(dirBuff) == GET_WORKING_DIRECTORY_SUCCESSFUL);

		else cout << "?";

		cout << hostname << "@" << user << dirBuff << '>';
		flush(cout);

		string line;
		if (!readInputLine(line)) break;

		// switch to normal input mode
		Shell::setMode(SHELL_MODE_DEFAULT);
		Shell::setEcho(true);

		// set std IO
		Fd inStdio[3];
		inStdio[0] = STDIN_FILENO;
		inStdio[1] = STDOUT_FILENO;
		inStdio[2] = STDERR_FILENO;

		Pid taskPid;
		Fd outStdio[3];

		// exec process
		SpawnStatus status = SpawnPOI(line.c_str(), "", path.c_str(), SECURITY_LEVEL_APPLICATION, &taskPid, outStdio, inStdio);

		// check result
		if (status == SPAWN_STATUS_SUCCESSFUL) 
		{
			Shell::setControlProcess(taskPid);
			Join(taskPid);
			Shell::setControlProcess(-1);
		} 

		else 
		{
			cout << (char) 27 << "[31m";
			if (status == SPAWN_STATUS_FORMAT_ERROR) cout << "Format error" << endl;

			else if (status == SPAWN_STATUS_IO_ERROR) cout << "I/O error" << endl;

			else cout << "Unknown error" << endl;
			cout << (char) 27 << "[0m";
			flush(cout);
		}
	}

	delete dirBuff;
}

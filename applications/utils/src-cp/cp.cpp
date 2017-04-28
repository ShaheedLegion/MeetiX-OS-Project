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

#include <eva.h>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		string readFile = argv[1];

		ifstream infile(readFile);
		if (!infile.good())
		{
			cerr << "Unable to open file" << endl;
			return -1;
		}

		if (argc > 2)
		{
			string argm = argv[2];
			string file = argm.substr(argm.find_last_of("/") + 1);
			string path = argm.substr(0, argm.find_last_of("/"));

			SetWorkingDirectory(path.c_str());

			ofstream outfile(file);

			char c;
			while (!infile.eof())
			{
				if (!infile.get(c)) break;

				outfile.put(c);
			}
		}

		else
		{
			cerr << "error: specify second path argument" << endl;
			return -1;
		}
	}

	else
		cerr << "usage: cp /path/to/readFile /path/to/copyFile" << endl;

	return 0;
}
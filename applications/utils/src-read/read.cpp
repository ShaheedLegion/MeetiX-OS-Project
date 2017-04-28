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

#include <string>
#include <iostream>
#include <fstream>

using namespace std;

/**
 *
 */
int main(int argc, char *argv[]) 
{
	if (argc > 1)
	{	
		string file = argv[1];
	
		ifstream infile(file);
		if (!infile.good())
		{
			cerr << "Impossibile aprire il file" << endl;
			return -1;
		}

		char c;
		while (infile.get(c))
			cout << c;
	}

	else 
		cerr << "usage: read <filename>" << endl;

	return 0;
}
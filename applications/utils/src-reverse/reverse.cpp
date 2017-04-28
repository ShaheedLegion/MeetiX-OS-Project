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

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <eva.h>

/**
 *
 */
int main(int argc, char** argv) 
{
	for (std::string line; std::getline(std::cin, line);) 
	{
		size_t linelen = line.length();
		char *rev = new char[linelen + 1];
		
		for (int i = 0; i < linelen; i++) 
		{
			rev[i] = line[linelen - i - 1];
		}
		rev[linelen] = 0;
		printf("%s\n", rev);
	}

}

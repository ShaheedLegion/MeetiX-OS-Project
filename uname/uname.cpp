/*
* MeetiX OS By MeetiX OS Project [Marco Cicognani & D. Morandi]
* 
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (char *argumentat your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "uname.hpp"

#include <MX.hpp>
#include <iostream>
#include <fstream>

#include <stdio.h>
#include <ghost.h>
#include <stdint.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
	if (argc > 1)
	{
		char *arg = argv[1];

		if (!strcmp(arg, "--help") || !strcmp(arg, "-h"))
		{
			help();
		}

		else if (!strcmp(arg, "-a"))
		{
			g_syscall_request_spechs sysSpechs = g_request_spechs();

			cout << "Kernel:" << endl;
			cout << "   EvaLoader v" << sysSpechs.LversionMJ << "." << sysSpechs.LversionMN << "." << sysSpechs.LversionPT << endl;
			cout << "   EvangelionNG v" << sysSpechs.versionMJ << "." << sysSpechs.versionMN << "." << sysSpechs.versionSB << "." << sysSpechs.versionPT << endl << endl;
			
			cout << "System:" << endl;
			cout << "   Free RAM: " << sysSpechs.RAM << " Mb" << endl;
			cout << "   Cpu: " << sysSpechs.cpuVendor << endl;
			cout << "   Cpu Type: " << sysSpechs.cpuType << " (" << sysSpechs.numberOfCore << " core)" << endl;

		}

		else if (!strcmp(arg, "-th"))
		{
			std::ifstream techFile("/sys/lib/uname/tech");
			
			if (techFile.good()) 
			{
				std::string File((std::istreambuf_iterator<char>(techFile)), std::istreambuf_iterator<char>());
				cout << File << endl;;
			}

			else
				cerr << "TechFile Presentation non found" << endl;
		}

		else
		{
			cerr << "Unrecognized option" << endl;
			help();
			return 1;
		}
	}

	else
	{ 
		cerr << "    MeetiX OS " << MeetiXversion << "  [Copyright (C) 2016, MeetiX OS Project]" << endl;
	}

	return 0;
}

void help()
{
	cout << "Uname v" << MAJOR << "." << MINOR <<
			"	use: 'uname -a'  Evangelion Kernel version\n"
			"	use: 'uname -th' for tech spech of Evangelion and MeetiX\n";
}

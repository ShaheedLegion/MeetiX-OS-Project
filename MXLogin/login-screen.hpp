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

#ifndef _LOGIN_SCREEN_
#define _LOGIN_SCREEN_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ghostuser/utils/property_file_parser.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

class Login_t
{
public:

	Login_t()
	{

	}

	static bool research_access(std::string username, std::string password)
	{
		std::ifstream cred("/usr/crd");

		// parse it		
		g_property_file_parser parser(cred);
		auto properties = parser.getProperties();

		std::string user;
		std::string passwd;

		for (auto entry : properties)
		{
			std::stringstream crd;
			crd << entry.second;

			if (entry.first == "username")
			{
				crd >> user;
			}

			else if (entry.first == "password")
			{
				crd >> passwd;
			}
		}

		if (username == user && passwd == password)
			return true;

		else 
			return false;
	}

};

#endif

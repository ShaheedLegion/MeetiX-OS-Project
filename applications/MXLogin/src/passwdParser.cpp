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

#include <MXLogin.hpp>
#include <mxuser/utils/property_file_parser.hpp>
#include <mxuser/utils/environment.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <eva.h>

using namespace std;

/*
 *	login constructor
 */
Login::Login()
{
	// crearing field
	rootUser.erase();
	rootPassword.erase();
	
	user.erase();
	passwd.erase();
}

/*
 *	return if provided username and password are correct
 */
bool Login::researchAccess(const string &username, const string &password)
{
	// open credential file
	ifstream cred("/usr/crd");

	// parse it		
	PropertyFileParser parser(cred);
	map<string, string> properties = parser.getProperties();

	// retrivie properties
	for (pair<string, string> entry : properties)
	{
		stringstream crd;
		crd << entry.second;

		if (entry.first == "rootUsername") crd >> rootUser;
		else if (entry.first == "rootPassword") crd >> rootPassword;
		else if (entry.first == "userName") crd >> user;
		else if (entry.first == "passWord") crd >> passwd;
	}

	// closing credential file
	cred.close();

	// test associated username and password
	if (username == rootUser && password == rootPassword)
	{
		logged(username);
		return true;
	}

	else if (username == user && password == passwd)
	{
		logged(username);
		return true;
	}

	else return false;
}

/*
 *	write on environment file logged user
 */
void Login::logged(const string &loggedUser)
{
	Environment::setEnvVar("USER", loggedUser);
}
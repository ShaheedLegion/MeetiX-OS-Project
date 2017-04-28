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

#include <login.hpp>
#include <screen.hpp>
#include <headless_screen.hpp>
#include <candyshell.hpp>

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <mxuser/utils/property_file_parser.hpp>
#include <mxuser/utils/environment.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>

#include <eva.h>
#include <mxuser/utils/utils.hpp>
#include <mxuser/utils/logger.hpp>
#include <mxuser/io/keyboard.hpp>

/**
 *	constructor
 */
Login::Login()
{
	rootUser.clear();
	rootPassword.clear();
	
	user.clear();
	passwd.clear();

	createAmbient();
}

/**
 *	main of login
 */
void Login::LoginEntry()
{
	Login *acc = new Login();

	TaskRegisterID("CandyText/Login");
	acc->loginLoop();
}

/**
 *	called when in text shell type "lock"
 */
void Login::lockLogin()
{
	CandyShell *lockLogin = new CandyShell();

	while (true)
	{
		createAmbient();
		ShellInputStatus st;

		user = Environment::getLoggedUser();

		out->write(' ' + user, Color::color(SC_STD_BACK, SC_RED));
		out->updateCursor();

		passwd.clear();
		
		out->write("\n PassWord: ", Color::color(SC_STD_BACK, SC_LGRAY));
		out->updateCursor();
		passwd = lockLogin->readInput(passwd, out, &st, 0, false);

		if (researchCredential(user, passwd)) break;

		else
		{
			out->write("\n Wrong PassWord", Color::color(SC_STD_BACK, SC_RED));
			out->updateCursor();
			Sleep(750);
		}
	}

	delete lockLogin;
}

/**
 *	login loop	
 */
void Login::loginLoop()
{
	CandyShell *loginShell = new CandyShell();

	while (true)
	{
		createAmbient();
		ShellInputStatus status;

		user.clear();
		
		out->write(" UserName: ", Color::color(SC_STD_BACK, SC_LGRAY));
		out->updateCursor();
		user = loginShell->readInput(user, out, &status, 0);
 		
		passwd.clear();
		
		out->write("\n PassWord: ", Color::color(SC_STD_BACK, SC_LGRAY));
		out->updateCursor();
		passwd = loginShell->readInput(passwd, out, &status, 0, false);

		if (researchCredential(user, passwd)) break;

		else
		{
			out->write("\n Wrong UserName or PassWord", Color::color(SC_STD_BACK, SC_RED));
			out->updateCursor();
			Sleep(750);
		}
	}

	delete loginShell;
}

/**
 *	create ambient	
 */
void Login::createAmbient()
{
	out = new HeadlessScreen();

	out->clean();
	out->activate();

	loadLogo(logoPath);
}

/**
 *	load logo	
 */
void Login::loadLogo(const char *path)
{
	ifstream logoptfile(path);
	if (logoptfile.good()) 
	{
		string loginlogo((istreambuf_iterator<char>(logoptfile)), istreambuf_iterator<char>());
		out->write(loginlogo, Color::color(SC_STD_BACK, SC_GREEN));

		out->write("\n", Color::color(SC_STD_BACK, SC_LGRAY));
		logoptfile.close();
	}
}

/**
 *	research imitted credential
 */
bool Login::researchCredential(const string &username, const string &password)
{
	// open password file
	ifstream cred("/usr/crd");

	// parse it		
	PropertyFileParser parser(cred);
	map<string, string> properties = parser.getProperties();

	// find
	for (pair<string, string> entry : properties)
	{
		if (entry.first == "rootUsername") rootUser = entry.second;
		else if (entry.first == "rootPassword") rootPassword = entry.second;
		else if (entry.first == "userName") user = entry.second;
		else if (entry.first == "passWord") passwd = entry.second;
	}

	// closing
	cred.close();

	// test
	if (username == rootUser)
	{
		if (password == rootPassword)
		{
			logged(username);
			return true;
		}

		else return false;
	}

	else if (username == user)
	{
		if (password == passwd)
		{
			logged(username);
			return true;
		}

		else return false;
	}

	else return false;
}

/**
 *	write environment variable	
 */
void Login::logged(const string &loggedUser)
{
	Environment::setEnvVar("USER", loggedUser);
}
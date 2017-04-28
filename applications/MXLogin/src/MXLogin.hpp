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

#include <string>

/*
 *	provide basic logger for login
 */
class Login
{
private:
	std::string rootUser;
	std::string rootPassword;

	std::string user;
	std::string passwd;

	/*
	 *	write on environment file logged user
	 */
	void logged(const std::string &loggedUser);

public:
	/*
	 *	class constructor
	 */
	Login();
	
	/*
	 *	return if provided username and password are correct
	 */
	bool researchAccess(const std::string &username, const std::string &password);
};

/*
 *	this method is called when enter or button login is pressed
 */
static void loginToMeetiX();

#endif

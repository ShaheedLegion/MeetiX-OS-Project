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

#include "login-screen.hpp"

#include <ghostuser/ui/ui.hpp>
#include <ghostuser/ui/window.hpp>
#include <ghostuser/ui/label.hpp>
#include <ghostuser/ui/button.hpp>
#include <ghostuser/ui/textfield.hpp>
#include <ghostuser/ui/action_listener.hpp>

static g_window *loginWindow;
static g_textfield *usernameField;
static g_textfield *passwordField;
g_rectangle std_bounds(1680 / 2 - 250, 1050 / 2 - 100, 420, 170);
g_rectangle err_bounds(1680 / 2 - 250, 1050 / 2 - 100, 420, 210);
/**
 *
 */
class login_button_action_listener_t : public g_action_listener 
{
public:

	/**
	 *
	 */
	virtual void handle_action() 
	{
		std::string username;
		std::string password;

		username = usernameField->getTitle();
		password = passwordField->getTitle();

		if (Login_t::research_access(username, password))
		{
			loginWindow->setVisible(false);
			return;
		}

		else
		{
			g_label *wrong = g_label::create();
			wrong->setBounds(g_rectangle(90, 120, 400, 30));
			wrong->setTitle("Wrong UserName or PassWord!");
			loginWindow->setBounds(err_bounds);
			loginWindow->addChild(wrong);

			g_sleep(1500);
			wrong->setVisible(false);

			loginWindow->setBounds(std_bounds);
		}
	}
};

/**
 *
 */
int main(int argc, char *argv[]) 
{
	g_ui_open_status open_stat = g_ui::open();

	if (open_stat == G_UI_OPEN_STATUS_SUCCESSFUL) 
	{
		g_task_register_id("MXLogin");
	
		loginWindow = g_window::create();

		loginWindow->setTitle("MeetiX OS Login");

		g_label *usernameLabel = g_label::create();
		usernameLabel->setBounds(g_rectangle(35, 15, 70, 30));
		usernameLabel->setTitle("UserName");
		loginWindow->addChild(usernameLabel);

		usernameField = g_textfield::create();
		usernameField->setBounds(g_rectangle(15, 45, 120, 30));
		loginWindow->addChild(usernameField);

		g_label *passwordLabel = g_label::create();
		passwordLabel->setBounds(g_rectangle(285, 15, 70, 30));
		passwordLabel->setTitle("PassWord");
		loginWindow->addChild(passwordLabel);

		passwordField = g_textfield::create();
		passwordField->setBounds(g_rectangle(260, 45, 120, 30));
		passwordField->setSecure(true);
		loginWindow->addChild(passwordField);

		g_button *loginButton = g_button::create();
		loginWindow->addChild(loginButton);
		loginButton->setBounds(g_rectangle(100, 80, 200, 30));
		loginButton->setTitle("Login");

		loginButton->setActionListener(new login_button_action_listener_t());

		loginWindow->setBounds(std_bounds);

		loginWindow->setResizable(false);
		loginWindow->setVisible(true);


		uint8_t blocker = true;
		g_atomic_block(&blocker);	
	}

	return 0;
}

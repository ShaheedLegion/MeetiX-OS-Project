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

#include "MXLogin.hpp"

#include <list>

#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/label.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/textfield.hpp>
#include <mxuser/ui/geoshape.hpp>
#include <mxuser/ui/action_listener.hpp>
#include <mxuser/ui/key_listener.hpp>

#include <mxuser/io/keyboard.hpp>
#include <mxuser/io/files/file_log.hpp>
#include <mxuser/utils/environment.hpp>
#include <mxuser/utils/time.hpp>
#include <mxuser/tasking/lock.hpp>

using namespace std;

// object for login shell
static Login *loginShell;

// main graphic form
static Geoshape *loginWindow;

// centre of login
static Geoshape *loginRectangle;
static Textfield *usernameField;
static Textfield *passwordField;
static Button *loginButton;

// info labels
static Label *info;
static Label *info2;
static Label *hourInfo;

// resolution of screen
static Dimension resolution;

// blocker for main loop
bool iteratorBlock = false;

// input container
static list<KeyInfo> inputBuffer;
static uint8_t inputBufferEmpty = true;
static Lock locker;

/**
 *	listener for key events
 */
class InputKeyListener : public KeyListener 
{
public:
	virtual void handleKeyEvent(KeyEvent &e) 
	{
		locker.lock();
		inputBuffer.push_back(Keyboard::fullKeyInfo(e.info));
		inputBufferEmpty = false;
		locker.unlock(); 
	}
};

/*
 *	listener of login button
 */
class LoginButtonActionListener : public ActionListener 
{
public:
	/**
	 *  virtual method of ActionListener that is called when button is cliked
	 */
	virtual void handleAction() { loginToMeetiX(); }
};

/*
 *	this method is called when enter or button login is pressed
 */
static void loginToMeetiX()
{
	// creating string variables for isereted username and password
	string username;
	string password;

	// get username and password
	username = usernameField->getTitle();
	password = passwordField->getTitle();

	// test credetial
	if (loginShell->researchAccess(username, password))
	{
		// if test pass invisibilize al components
		loginWindow->setVisible(false);
		loginRectangle->setVisible(false);

		FileLog::flog("Logging as: " + username);

		// exec mx shell to start ui components
		Spawn("/bin/mx", "-s uiStart.sh", "/sys/init/gui/", SECURITY_LEVEL_APPLICATION);
		
		// unlock
		iteratorBlock = true;
		inputBufferEmpty = false;

		// deleting all objects
		delete loginShell;
		loginWindow->remove();
		usernameField->remove();
		passwordField->remove();
		loginButton->remove();
		info->remove();
		info2->remove();

		// set iteratorBlock to true, task iteratorBlock
		locker.unlock();
	}

	else
	{
		loginButton->setTitle("Wrong UserName or PassWord!");
		loginButton->setColor(ARGB(200, 230, 230, 230), ARGB(255, 255, 0, 0));
		usernameField->setTitle("");
		passwordField->setTitle("");

		Sleep(1500);

		loginButton->setColor(ARGB(150, 0, 0, 0), ARGB(255, 255, 255, 255));
		loginButton->setTitle("Login");
	}
}

/*
 *	secondary thread: responsible of time label
 */
void timeThread()
{
	// registering name of task
	TaskRegisterID("MXLogin/timeThread");

	// creating varible to time struct
	TimeDriverCall *time;
	
	// infinite iteration, end when login terminate
	while (true)
	{
		// call timedriver to fill time struct
		time = MXtime::getTime();

		// pack data
		stringstream hours;
		hours << time->hour << ":" << time->minute << ":" << time->second;

		// and write in hour label
		hourInfo->setTitle(hours.str());

		// wait one second
		Sleep(1000);
	}

	delete time;
}

/*
 *	instantiate objects
 */
void initializeObjects()
{
	// create object to login class
	loginShell = new Login();

	// creating geoshape objects
	loginWindow = Geoshape::create();
	loginRectangle = Geoshape::create();

	// creating labels objects
	info = Label::create();
	info2 = Label::create();
	hourInfo = Label::create();

	// creating textfields objects
	usernameField = Textfield::create();
	passwordField = Textfield::create();

	// creting buttons
	loginButton = Button::create();
}

/*
 * configure main window
 */
void configureMainWindow()
{
	// setting main geoshape with dimension of screen
	loginWindow->setBounds(Rectangle(0, 0, resolution.width, resolution.height));
	loginWindow->setColor(ARGB(120, 180, 180, 180), ARGB(255, 0, 255, 0));
	loginWindow->setTitle("Welcome in MeetiX OS");
}

/*
 *	configure the info labels
 */
void configureLabels()
{
	// setting label with hostname and os version
	info->setBounds(Rectangle(10, resolution.height - 30, 200, 30));
	info->setFont("consolas");
	info->setColor(0, RGB(255, 255, 255));
	info->setTitle("Version " + Environment::getEnvVar("VERSION"));
	loginWindow->addChild(info);

	// setting hour label
	hourInfo->setFont("consolas");
	hourInfo->setBounds(Rectangle(0, 10, 280, 30));
	hourInfo->setTitleAlignment(TextAlignment::CENTER);
	hourInfo->setFontSize(20);
	hourInfo->setColor(0, RGB(255, 255, 255));

	// setting info labels
	info2->setBounds(Rectangle(resolution.width - 90, resolution.height - 30, 200, 30));
	info2->setFont("consolas");
	info2->setColor(0, RGB(255, 255, 255));
	info2->setTitle(Environment::getHostname() + "//");
	loginWindow->addChild(info2);
}

/*
 *	configure the textfields
 */
void configureActionComponents()
{
	// setting textfield for username
	usernameField->setBounds(Rectangle(15, 50, 250, 30));
	usernameField->setColor(ARGB(20, 0, 0, 0), ARGB(255, 255, 255, 255));
	usernameField->setGhostTitle("Enter your UserName");

	// setting textfield for password
	passwordField->setBounds(Rectangle(15, 100, 250, 30));
	passwordField->setColor(ARGB(20, 0, 0, 0), ARGB(255, 255, 255, 255));
	passwordField->setGhostTitle("Enter your PassWord");
	passwordField->setSecure(true);

	// setting button
	loginButton->setBounds(Rectangle(15, 150, 250, 30));
	loginButton->setColor(ARGB(150, 0, 0, 0), ARGB(255, 255, 255, 255));
	loginButton->setTitle("Login");
	loginButton->setActionListener(new LoginButtonActionListener());
}

/*
 *
 */
KeyInfo readInput() 
{
	// wait for input
	if (inputBuffer.size() == 0) AtomicBlock(&inputBufferEmpty);

	// lock thread
	locker.lock();

	KeyInfo result = inputBuffer.front(); // safety copy
	inputBuffer.pop_front(); // clear list

	// unlock 
	if (inputBuffer.size() == 0) inputBufferEmpty = true;

	locker.unlock();

	return result;
}

/*
 * do animation with "MeetiX OS" logo
 */
void paintAnimation()
{
	Label *animation = Label::create();
	animation->setBounds(Rectangle(0, 250, resolution.width, 200));
	animation->setTitleAlignment(TextAlignment::CENTER);
	animation->setFont("Xcelsion");
	animation->setFontSize(130);
	animation->setTitle("MeetiX OS");
	animation->setColor(0, RGB(0, 200, 0));
	loginWindow->addChild(animation);
	for (int yPosition = 250; yPosition >= 200; yPosition--)
	{
		// show logo animation only in even position
		if ((yPosition % 2) == 0) animation->setBounds(Rectangle(0, yPosition, resolution.width, 200));
			// sleep
			Sleep(1);
	}
}

/*
 *	main loop of login
 */
static void mainLoop()
{
	uint8_t lastFocused = false;

	// main loop of login
	while (!iteratorBlock)
	{
		// read from keyboard
		KeyInfo k = readInput();
		
		// get event of tab pressing to set opposite textfield focused
		if (k.pressed)
		{
			if (k.key == "KEY_TAB")
			{
				if (!lastFocused)
				{
					usernameField->setFocus(false);
					passwordField->setFocus(true);
					lastFocused = true;
				}

				else
				{
					passwordField->setFocus(false);
					usernameField->setFocus(true);
					lastFocused = false;
				}
			}

			else if (k.key == "KEY_ENTER") loginToMeetiX();
		}
	}
}

/*
 *	MXLogin, graphical login of MeetiX OS
 */
int main(int argc, char *argv[]) 
{
	// ask windowserver zipNET to open a comunication channel
	UiOpenStatus openStat = UI::open();

	// if channel is open 
	if (openStat == UI_OPEN_STATUS_SUCCESSFUL) 
	{
		// get resolution of screen
		resolution = UI::getResolution();

		// instantiate objects
		initializeObjects();

		// configure main geoshape
		configureMainWindow();

		// configure labels
		configureLabels();
		
		// setting login geoshape
		loginRectangle->setVisible(false);
		loginRectangle->setBounds(Rectangle(resolution.width / 2 - 140, resolution.height / 2 - 115, 280, 190));
		loginRectangle->setColor(ARGB(120, 20, 150, 20), ARGB(255, 0, 0, 0));
		loginRectangle->setListener(UI_COMPONENT_EVENT_TYPE_KEY, new InputKeyListener());
		loginWindow->setListener(UI_COMPONENT_EVENT_TYPE_KEY, new InputKeyListener());

		// run as a background thread the paint animation method
		CreateThread((void*) &paintAnimation);

		// configure button and textfields
		configureActionComponents();

		// add to screen componets
		loginRectangle->setVisible(true);

		// exec timeThread in background
		CreateThread((void*) &timeThread);

		// add to login rectangle components
		loginRectangle->addChild(hourInfo);
		loginRectangle->addChild(usernameField);
		loginRectangle->addChild(passwordField);
		loginRectangle->addChild(loginButton);

		// load layout
		Keyboard::loadLayout("it-EU");
		CreateThread((void*)mainLoop);

		uint8_t lock = true;
		AtomicBlock(&lock);
	}

	return 0;
}

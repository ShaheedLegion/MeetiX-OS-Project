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

#include <desktop.hpp>
#include <mxuser/ui/about.hpp>
#include <mxuser/utils/environment.hpp>
#include <mxuser/utils/property_file_parser.hpp>
#include <mxuser/io/files/file_log.hpp>
#include <mxuser/io/keyboard.hpp>
#include <mxuser/ui/menu.hpp>
#include <sstream>
#include <fstream>
#include <list>
#include <map>

using namespace std;

// flag for pressed start menu
static bool pressed = false;

// locker of application
static uint8_t blocker = true;

// object where we draw components
static Geoshape *menuTab;

// start menu button
static Button *menuButton;

// environment user
static std::string loggedUser;

// menu of start
static ButtonMenu start;

// label for display
static Label *taskLabel;
static Label *hourLabel;
static Label *memLabel;

// input container
static list<KeyInfo> inputBuffer;
static uint8_t inputBufferEmpty = true;
static Lock locker;

/**
 *	key listener for events
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
 *	destruct class
 */
CandyGUI::~CandyGUI()
{
	menuTab->remove();
	menuButton->remove();
	taskLabel->remove();
	hourLabel->remove();
	memLabel->remove();
	about->remove();
}

/**
 *	Open comunication to windowserver
 */
bool CandyGUI::init()
{
	// open communication to ui
	UiOpenStatus openStat = UI::open();

	// if open success
	if (openStat == UI_OPEN_STATUS_SUCCESSFUL) return true;
	
	return false;
}

/**
 *	configure layout of UI with configuration file and provided resolution
 */
void CandyGUI::configureUi(std::string pathToConfiguration, Dimension resolution)
{
	// copy to internal resolution provided dimension
	this->resolution = resolution;

	// load keyboard layout
	Keyboard::loadLayout("it-EU");

	// keep configuration file
	std::ifstream conf(pathToConfiguration);

	// parse it		
	PropertyFileParser parser(conf);
	configuration = parser.getProperties();

	// closing file
	conf.close();

	// creating bar
	Bar = Geoshape::create();
	Bar->setColor(ARGB(100, 0, 0, 0), ARGB(255, 255, 255, 255));
	Bar->setListener(UI_COMPONENT_EVENT_TYPE_KEY, new InputKeyListener());

	if (configuration["UiStyle"] == "GNOME")
	{
		// set bounds
		Bar->setBounds(Rectangle(0, 0, resolution.width, 30));

		// set mode
		mode = GNOME;
		Environment::setEnvVar("UISTYLE", "GNOME");
	}

	else if (configuration["UiStyle"] == "KDE")
	{
		// set bounds
		Bar->setBounds(Rectangle(0, resolution.height - 30, resolution.width, 30));

		// if mode is kde deactivate candydock thread if is active
		if (configuration["CandyDockThread"] == "true") configuration["CandyDockThread"] = "false";

		// set mode
		mode = KDE;
		Environment::setEnvVar("UISTYLE", "KDE");
	}
}

/**
 *	create and setup start button
 */
void CandyGUI::setMenuButton()
{
	//create and set menù button
	menuButton = Button::create();

	buttons.add("menu", menuButton, []
	{ 
		if (!pressed)
		{
			pressed = true;
			
			if (loggedUser == "root")
			{
				menuButton->setColor(ARGB(255, 255, 10, 10), ARGB(255, 0, 0, 0));
				menuTab->setColor(ARGB(100, 0, 0, 0), ARGB(255, 255, 0, 0));
			}

			else
			{
				menuButton->setColor(ARGB(255, 10, 255, 10), ARGB(255, 0, 0, 0));
				menuTab->setColor(ARGB(100, 0, 0, 0), ARGB(255, 0, 255, 0));
			}
			
			menuTab->setVisible(true);
		}

		else
		{ 
			pressed = false;
			menuButton->setColor(ARGB(255, 10, 200, 10), ARGB(255, 0, 0, 0));
			menuTab->setVisible(false);
		} 
	});

	buttons.configure("menu", Rectangle(0, 0, 72, 30), "", "/sys/graphics/zipNET/components/mx.png", Point(0, 0), ARGB(255, 10, 200, 10), ARGB(255, 0, 0, 0));
	buttons.show(Bar);
}

/**
 *	initialize content of start menu
 */
void CandyGUI::setMenuTab()
{
	//create menutab
	menuTab = Geoshape::create();
	menuTab->setVisible(pressed);
	
	loggedUser = Environment::getLoggedUser();
	std::stringstream Title;

	Title << "MeetiX OS " << Environment::getEnvVar("VERSION") << " [" << loggedUser << "]";

	menuTab->setTitle(Title.str());

	if (mode == GNOME) menuTab->setBounds(Rectangle(0, 30, 270, 280));

	else if (mode == KDE) menuTab->setBounds(Rectangle(0, resolution.height - 310, 270, 280));
}

/**
 *	initialize label to display memory occupation
 */
void CandyGUI::setMemLabel()
{
	memLabel = Label::create();
	memLabel->setBounds(Rectangle(135, 252, 135, 30));
	memLabel->setColor(0, RGB(255, 255, 255));
	memLabel->setTitleAlignment(TextAlignment::CENTER);
	menuTab->addChild(memLabel);
}

/**
 *	initialize label to display current time
 */
void CandyGUI::setHourLabel()
{
	//create and set hour label
	hourLabel = Label::create();
	hourLabel->setBounds(Rectangle(resolution.width - 145, 0, 140, 30));
	hourLabel->setColor(0, RGB(255, 255, 255));
	hourLabel->setTitleAlignment(TextAlignment::RIGHT);
	hourLabel->setFont("Xcelsion");
	hourLabel->setFontSize(22);
	Bar->addChild(hourLabel);	
}

/**
 *	initialize label to display windows name
 */
void CandyGUI::setTaskLabel()
{
	taskLabel = Label::create();
	taskLabel->setBounds(Rectangle(75, -3, resolution.width - 155, 30));
	taskLabel->setColor(0, RGB(255, 255, 255));
	Bar->addChild(taskLabel);
}

/*
 *	show about 
 */
class MeetiXOSAbout : public ActionListener
{
public:
	virtual void handleAction() { aboutMeetiXOS(); }
};

/**
 *	create list of button menu into start space
 */
void CandyGUI::createMenu()
{
	// get bounds of menu
	Rectangle menuBounds = menuTab->getBounds();

	// crete and configure menu object with script file
	start.create("/home/" + loggedUser + "/.desktop", menuBounds);
	start.show(menuTab);

	// create and setup about button
	about = Button::create();
	menuTab->addChild(about);
	about->setBounds(Rectangle(0, 250, 270, 30));
	about->setColor(ARGB(120, 0, 200, 0), RGB(255, 255, 255));
	about->setTitleAlignment(TextAlignment::LEFT);
	about->setFont("consolas");
	about->setTitle("  About MeetiX OS");
	about->setActionListener(new MeetiXOSAbout());
}

/**
 *	call all previous method
 */
void CandyGUI::createComponents()
{
	setMenuButton(); // create and set menù button
	setMenuTab();	// create menu tab
	createMenu();	// create app button into menu
	setMemLabel();	// create label for memory usage indicator
	setHourLabel();	// create and set hour label
	setTaskLabel();	// create task label
}

/*
 *	read from KeyEvent 
 */
KeyInfo CandyGUI::readInput() 
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

/**
 *	mainloop, start secondary thread if there are set active in configuration, than wait for events
 */
void CandyGUI::mainLoop()
{
	// exec asincronous thread if in configuration are enabled
	if (configuration["TaskManagerThread"] == "true") UI::registerTaskManager(taskLabel, taskLabel->getBounds());
	if (configuration["HourManagerThread"] == "true") CreateThreadD((void*) &SecondaryThread::HourManagerThread, (void*) hourLabel);
	if (configuration["MemoryUsageThread"] == "true") CreateThreadD((void*) &SecondaryThread::MemoryUsageThread, (void*) memLabel);
	if (configuration["CandyDockThread"] == "true") CreateThreadD((void*) &SecondaryThread::CandyDockThread, (void*) &resolution);

	// main loop of gui
	while (true)
	{
		// read pressed keys
		KeyInfo key = readInput();

		if (key.pressed)
		{
			// reading combination
			if (key.alt && key.key == "KEY_T") Spawn("/bin/CandyShell", "", "/", SECURITY_LEVEL_APPLICATION);
			else if (key.alt && key.key == "KEY_S") Spawn("/bin/CandyNote", "-new", "/", SECURITY_LEVEL_APPLICATION);
		}
	}
}
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

#include <UItweak.hpp>
#include <sstream>
#include <mxuser/utils/property_file_parser.hpp>

Textfield *winColorField;
Textfield *backgroundField;
Textfield  *buttonField;

class UItweakListener : public ActionListener
{
public:

	virtual void handleAction()
	{
		UItweak::ThemeConfig();
	}
};

class ExitListerner : public ActionListener
{
public:

	virtual void handleAction()
	{
		winColorField->remove();
		backgroundField->remove();
		buttonField->remove();
		Kill(GetPid());
	}
};

bool UItweak::init()
{
	UiOpenStatus openStat = UI::open();
	
	if (openStat == UI_OPEN_STATUS_SUCCESSFUL)
		return true;

	else
		return false; 
}

void UItweak::createUItweakWindow()
{
	window = Window::create();
	window->setBounds(Rectangle(600, 550, 350, 385));
	window->setTitle("CandyGui Tweak Tool");
	window->onClose([]
	{
		winColorField->remove();
		backgroundField->remove();
		buttonField->remove();
		Kill(GetPid());
	});

	window->setVisible(true);
	window->setResizable(true);
}

void UItweak::colorTweaker()
{
	winColorLabel = Label::create();
	winColorLabel->setBounds(Rectangle(5, 5, 330, 30));
	winColorLabel->setTitle("Write Color for windows and UI [color]");
	window->addChild(winColorLabel);

	winColorField = Textfield::create();
	winColorField->setBounds(Rectangle(5, 35, 315, 30));	
	window->addChild(winColorField);
}

void UItweak::backgroundTweaker()
{
	backgroundLabel = Label::create();
	backgroundLabel->setBounds(Rectangle(10, 70, 330, 30));
	backgroundLabel->setTitle("Write back name [name]");
	window->addChild(backgroundLabel);

	backgroundField = Textfield::create();
	backgroundField->setBounds(Rectangle(5, 100, 315, 30));	
	window->addChild(backgroundField);
}

void UItweak::buttonTweaker()
{
	buttonLabel = Label::create();
	buttonLabel->setBounds(Rectangle(10, 135, 330, 30));
	buttonLabel->setTitle("Write button color [color]");
	window->addChild(buttonLabel);

	buttonField = Textfield::create();
	buttonField->setBounds(Rectangle(5, 165, 315, 30));	
	window->addChild(buttonField);
}

void UItweak::createApplyButton()
{
	applyButton = Button::create();
	applyButton->setBounds(Rectangle(60, 260, 200, 30));
	applyButton->setTitle("Apply Modify");
	applyButton->setActionListener(new UItweakListener());
	window->addChild(applyButton);
}

void UItweak::createExitButton()
{
	exit = Button::create();
	exit->setBounds(Rectangle(60, 290, 200, 30));
	exit->setTitle("Exit");
	exit->setActionListener(new ExitListerner());
	window->addChild(exit);
}

void UItweak::paintComponents()
{
	createUItweakWindow();
	colorTweaker();
	backgroundTweaker();
	buttonTweaker();

	createApplyButton();
	createExitButton();
}

void UItweak::ThemeConfig()
{
	//set graphics configuration dir
	SetWorkingDirectory("/sys/graphics/CandyGui/configuration/");

	//create a set of variables for new and hold setup
	string Color, BColor;
	string Background, BBackground;
	string Button, BButton;

	//read the hold file
	ifstream HoldTheme("theme.cfg");

	//parsing the file and assign hold value to hold setup variables
	PropertyFileParser parser(HoldTheme);
	auto properties = parser.getProperties();

	for (auto entry : properties)
	{
		stringstream tmp;
		tmp << entry.second;

		if (entry.first == "UIcolor") tmp >> BColor;

		else if (entry.first == "Background") tmp >> BBackground;

		else if (entry.first == "ButtonColor") tmp >> BButton;
	}
	HoldTheme.close();

	//retrivie new value to textfield
	Color = winColorField->getTitle();
	Background = backgroundField->getTitle();
	Button = buttonField->getTitle();

	//open theme configuration file in write mode
	ofstream NewTheme("theme.cfg");

	//write new file with new value if user have writed
	if (Color.size() > 0) NewTheme << "UIcolor=\"" << Color << "\"" << endl;

	else NewTheme << "UIcolor=\"" << BColor << "\"" << endl;

	
	if (Background.size() > 0) NewTheme << "Background=\"" << Background << "\"" << endl;

	else NewTheme << "Background=\"" << BBackground << "\"" << endl;
	
	
	if (Button.size() > 0) NewTheme << "ButtonColor=\"" << Button << "\"" << endl;

	else NewTheme << "ButtonColor=\"" << BButton << "\"" << endl;

	NewTheme.close();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * * *
* MeetiX OS By MeetiX OS Project [Marco Cicognani]                                    *
*                                                                                     *
* This program is free software; you can redistribute it and/or                       *
* modify it under the terms of the GNU General Public License                         *
* as published by the Free Software Foundation; either version 2                      *
* of the License, or (char *argumentat your option) any later version.                *
*                                                                                     *
* This program is distributed in the hope that it will be useful,                     *
* but WITHout ANY WARRANTY; without even the implied warranty of                      *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                       *
* GNU General Public License for more details.                                        *
*                                                                                     *
* You should have received a copy of the GNU General Public License                   *
* along with this program; if not, write to the Free Software                         *
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA      *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *  * * * * * * */

#include <eva.h>

#include <mxuser/utils/environment.hpp>
#include <mxuser/ui/ui.hpp>
#include <mxuser/ui/geoshape.hpp>
#include <mxuser/ui/window.hpp>
#include <mxuser/ui/button.hpp>
#include <mxuser/ui/actionlistener.hpp>
#include <mxuser/ui/label.hpp>

// display forms
static Geoshape *backGround;
static Window *mainWindow;

// labels that contains esplanation text
static Label *lb1;
static Label *lb2;
static Label *lb3;
static Label *lb4;
static Label *lb5;

// button for events
static Button *nextButton;
static Button *PrevButton;
// resolutions
static Dimension resolution;
static Rectangle windowDimension;

// bookmark
int page;

// waiter block
uint8_t blocker = true;

/*
 *	labels content for first line
 */
std::string rg1[] =
{
	"Hi " + Environment::getEnvVar("USER") + " welcome in MeetiX OS " + Environment::getEnvVar("VERSION"),
	"Okay, we can begin",
	"",
	"The graphic terminal CandyShell allows you to interact more deeply with the system",
	"",
};

/*
 *	labels content for second line
 */
std::string rg2[] =
{
	"I will guide you with a brief introduction to the main aspects of the graphical interface",
	"The OsmosUI has a KDE style, so you can find a bar in the bottom of the screen,",
	"Nella versione 2 la CandyGui è dotata di un nuovo modulo chiamato CandyDock,",
	"di eseguire comandi e lanciare nuove applicazioni emulando un ambiente Unix-Like",
	"Ricorda: MeetiX OS è proprietà del MeetiX OS Project",
};

/*
 *	labels content for third line
 */
std::string rg3[] =
{
	"in modo tu prenda confidenza con il sistema e le applicazioni; puoi uscire quando vuoi premendo la X",
	"partendo da sinistra, un menù con le applicazioni e il consumo di memoria in tempo reale.",
	"una Dock integrata per eseguire velocemente le applicazioni di MeetiX OS",
	"",
	"il cui team di sviluppo conta ancora poche persone, quindi sono ancora presenti diversi bug...",
};

/*
 *	labels content for fourth line
 */
std::string rg4[] =
{
	"MeetiX OS è un software parzialmente open source",
	" Al centro della barra è presente il task manager, che visualizza i nomi delle applicazioni in esecuzione",
	"",
	"",
	"",
};

/*
 *	labels content for fifth line
 */
std::string rg5[] =
{
	"t.me/MeetiXOSProject",
	"mentre in fondo a destra l'orologio scandito in ore:minuti:secondi",
	"",
	"",
	"provalo, e se si blocca riavvia la macchina virtuale",
};

/*
 *	show on labels provided index page
 */
static void showPage(int index)
{
	if (index == 1)
	{
		mainWindow->setBounds(windowDimension);
		backGround->setPNG("/usr/share/arrow_up.png", Point(resolution.width / 2 - 53, 0));

		lb1->setTitle(rg1[index]);
		lb2->setTitle(rg2[index]);
		lb3->setTitle(rg3[index]);
		lb4->setTitle(rg4[index]);
		lb5->setTitle(rg5[index]);
	}

	else if (index == 2)
	{
		mainWindow->setBounds(windowDimension);
		backGround->setPNG("/usr/share/arrow_down.png", Point(resolution.width / 2 - 53, resolution.height - 300));
		lb1->setTitle(rg1[index]);
		lb2->setTitle(rg2[index]);
		lb3->setTitle(rg3[index]);
		lb4->setTitle(rg4[index]);
		lb5->setTitle(rg5[index]);
	}

	else if (index == 3)
	{
		mainWindow->setBounds(windowDimension);
		backGround->setPNG("/usr/share/arrow_down.png", Point(resolution.width / 2 - 200, resolution.height - 300));
		lb1->setTitle(rg1[index]);
		lb2->setTitle(rg2[index]);
		lb3->setTitle(rg3[index]);
		lb4->setTitle(rg4[index]);
		lb5->setTitle(rg5[index]);

		nextButton->setColor(ARGB(180, 0, 200, 0), RGB(0, 0, 0));
		nextButton->setTitle("Next");
	}

	else if (index == 4)
	{
		mainWindow->setBounds(windowDimension);
		lb1->setTitle(rg1[index]);
		lb2->setTitle(rg2[index]);
		lb3->setTitle(rg3[index]);
		lb4->setTitle(rg4[index]);

		lb5->setColor(0, RGB(200, 0, 0));
		lb5->setTitle(rg5[index]);
		lb5->setColor(0, RGB(0, 0, 0));

		nextButton->setColor(RGB(200, 0, 0), RGB(0, 0, 0));
		nextButton->setFontSize(20);
		nextButton->setTitle("Exit");
		nextButton->setFontSize(14);
	}

	else if (index == 5) blocker = false;
}

/*
 *	handler for button next
 */
class ButtonNextListener : public ActionListener
{
public:
	virtual void handleAction() { showPage(++page); }
};

/*
 *	handler for button previous
 */
class ButtonPrevListener : public ActionListener
{
public:
	virtual void handleAction() { showPage(--page); }
};

/*
 * livewelcome, explain basic information for MeetiX OS GUI
 */
int main(int argc, char *argv[])
{
	// open comunication to window server
	UiOpenStatus openStat = UI::open();

	// if open succesful
	if (openStat == UI_OPEN_STATUS_SUCCESSFUL)
	{
		// get screen resolution and set window resolution
		resolution = UI::getResolution();
		windowDimension = Rectangle(resolution.width / 2 - 375, resolution.height / 2 - 225, 750, 450);

		// setting background geoshape
		backGround = Geoshape::create();
		backGround->setBounds(Rectangle(0, 30, resolution.width, resolution.height));
		backGround->setColor(ARGB(90, 0, 0, 0), RGB(255, 255, 255));

		// set frontend window
		mainWindow = Window::create();
		mainWindow->setBounds(windowDimension);
		mainWindow->setColor(RGB(0, 200, 0), RGB(255, 255, 255));
		mainWindow->setTitle("Live Welcome v1.0");
		mainWindow->setVisible(true);
		mainWindow->setResizable(false);
		mainWindow->onClose([]{ blocker = false; });

		// configuring label
		lb1 = Label::create();
		lb1->setBounds(Rectangle(0, 20, 750, 30));
		lb1->setTitleAlignment(TextAlignment::CENTER);
		mainWindow->addChild(lb1);

		// configuring label
		lb2 = Label::create();
		lb2->setBounds(Rectangle(0, 50, 750, 30));
		lb2->setTitleAlignment(TextAlignment::CENTER);
		mainWindow->addChild(lb2);

		// configuring label
		lb3 = Label::create();
		lb3->setBounds(Rectangle(0, 80, 750, 30));
		lb3->setTitleAlignment(TextAlignment::CENTER);
		mainWindow->addChild(lb3);

		// configuring label
		lb4 = Label::create();
		lb4->setBounds(Rectangle(0, 110, 750, 30));
		lb4->setTitleAlignment(TextAlignment::CENTER);
		mainWindow->addChild(lb4);

		// configuring label
		lb5 = Label::create();
		lb5->setBounds(Rectangle(0, 140, 750, 30));
		lb5->setTitleAlignment(TextAlignment::CENTER);
		mainWindow->addChild(lb5);

		// configuring button
		nextButton = Button::create();
		nextButton->setBounds(Rectangle(windowDimension.width / 2 - 200, 300, 400, 30));
		nextButton->setColor(ARGB(180, 0, 200, 0), RGB(0, 0, 0));

		nextButton->setTitle("Next");
		nextButton->setActionListener(new ButtonNextListener());
		mainWindow->addChild(nextButton);

		// configuring button
		PrevButton = Button::create();
		PrevButton->setBounds(Rectangle(windowDimension.width / 2 - 200, 350, 400, 30));
		PrevButton->setColor(ARGB(180, 0, 200, 0), RGB(0, 0, 0));

		PrevButton->setTitle("Previous");
		PrevButton->setActionListener(new ButtonPrevListener());
		mainWindow->addChild(PrevButton);

		// set page to first
		page = 0;

		// initialize labels
		lb1->setColor(0, RGB(0, 200, 0));
		lb1->setFontSize(30);
		lb1->setTitle(rg1[page]);
		lb1->setColor(0, RGB(0, 0, 0));
		lb1->setFontSize(14);

		lb2->setTitle(rg2[page]);
		lb3->setTitle(rg3[page]);
		lb4->setTitle(rg4[page]);

		lb5->setColor(0, RGB(200, 0, 0));
		lb5->setFontSize(20);
		lb5->setTitle(rg5[page]);
		lb5->setColor(0, RGB(0, 0, 0));
		lb5->setFontSize(14);

		// wait for events
		AtomicBlock(&blocker);

		// remove labels
		delete lb1;
		delete lb2;
		delete lb3;
		delete lb4;
		delete lb5;

		// remove buttons
		delete nextButton;
		delete PrevButton;

		// remove containers
		delete backGround;
		delete mainWindow;

		// close the interface connection
		UI::close();
	}
}

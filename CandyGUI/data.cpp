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
#include <MX.hpp>

bool pressed = false;
g_window *menuTab;

class CandyUImenuLister_t : public g_action_listener
{
public:
	virtual void handle_action()
	{
		if (!pressed)
		{
			pressed = true;
			menuTab->setBounds(g_rectangle(-10, 780, 250, 250));
			menuTab->setVisible(true);
		}

		else
		{ 
			pressed = false;
			menuTab->setVisible(false);
		}
	}
};

class FirstAppListener_t : public g_action_listener
{
public:
	virtual void handle_action()
	{
		g_pid pid = -1;
		g_spawn_p("/bin/CandyShell.mxe", "", "/", G_SECURITY_LEVEL_APPLICATION, &pid);
	}
};

class SecondAppListener_t : public g_action_listener
{
public:
	virtual void handle_action()
	{
		g_pid pid = -1;
		g_spawn_p("/bin/calculator.mxe", "", "/", G_SECURITY_LEVEL_APPLICATION, &pid);
	}
};

class ThirdAppListener_t : public g_action_listener
{
public:
	virtual void handle_action()
	{
		g_pid pid = -1;
		g_spawn_p("/bin/Medit.mxe", "", "/", G_SECURITY_LEVEL_APPLICATION, &pid);
	}
};

bool CandyGUI_t::init()
{
	g_task_register_id("CandyGui");

	auto open_stat = g_ui::open();

	if (open_stat == G_UI_OPEN_STATUS_SUCCESSFUL) 
		return true;

	else
		return false;
}

void CandyGUI_t::initCanvas()
{
	canvas = g_canvas::create();
	g_ui::register_desktop_canvas(canvas);
	canvas->setBounds(g_rectangle(0, 0, 1680, 1050));
}

void CandyGUI_t::setMenuButton()
{
	//create and set menù button
	menuButton = g_button::create();
	menuButton->setBounds(g_rectangle(0, 1020, 70, 30));
	menuButton->setTitle("MeetiX");
	menuButton->setActionListener(new CandyUImenuLister_t());
	canvas->addChild(menuButton);
}

void CandyGUI_t::setMenuTab()
{
	//create menutab
	menuTab = g_window::create();
	menuTab->setBounds(g_rectangle(-10, 780, 250, 250));
	menuTab->setTitle("MeetiX OS " + MeetiXversion);
	menuTab->setResizable(false);
}

void CandyGUI_t::setHourLabel()
{
	//create and set hour label
	hourLabel = g_label::create();
	hourLabel->setBounds(g_rectangle(1515, 1020, 100, 30));
	canvas->addChild(hourLabel);	
}

void CandyGUI_t::appButton()
{
	g_button *app1 = g_button::create();
	app1->setBounds(g_rectangle(0, 0, 245, 30));
	app1->setTitle("CandyShell");
	app1->setActionListener(new FirstAppListener_t());
	menuTab->addChild(app1);

	g_button *app2 = g_button::create();
	app2->setBounds(g_rectangle(0, 30, 245, 30));
	app2->setTitle("Calculator");
	app2->setActionListener(new SecondAppListener_t());
	menuTab->addChild(app2);

	g_button *app3 = g_button::create();
	app3->setBounds(g_rectangle(0, 60, 245, 30));
	app3->setTitle("Medit Editor");
	app3->setActionListener(new ThirdAppListener_t());
	menuTab->addChild(app3);
}

void CandyGUI_t::setTaskLabel()
{
	taskLabel = g_label::create();
	taskLabel->setBounds(g_rectangle(80, 1018, 1500, 30));
	canvas->addChild(taskLabel);
}

void CandyGUI_t::createComponents()
{

	//create and set menù button
	setMenuButton();

	//create menu tab
	setMenuTab();

	//create and set hour label
	setHourLabel();

	//create app button into menu
	appButton();

	//create task label
	setTaskLabel();
}

void CandyGUI_t::mainLoop()
{
	int x = 0;
	int y = 0;

	g_create_thread((void*) &SecondaryThread_t::TaskManagerThread);
	g_create_thread((void*) &SecondaryThread_t::HourManagerThread);

	while (true) 
	{
		auto bufferInfo = canvas->getBuffer();
		if (bufferInfo.buffer == 0) 
		{
			g_sleep(100);
			continue;
		}

		// get the surface ready and go:
		bufferSurface = cairo_image_surface_create_for_data((uint8_t*) bufferInfo.buffer, CAIRO_FORMAT_ARGB32, bufferInfo.width, bufferInfo.height, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, bufferInfo.width));
		cr = cairo_create(bufferSurface);

		// clear
		cairo_save(cr);
		cairo_set_source_rgba(cr, 0, 0, 0, 0);
		cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
		cairo_paint(cr);
		cairo_restore(cr);

		// draw a taskbar
		cairo_set_source_rgba(cr, 255, 255, 255, 0.4);
		cairo_rectangle(cr, 0, 1020, 1680, 30);
		cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
		cairo_fill(cr);

		x = x + 5;
		if (x > 1680) 
		{
			x = 0;
			y = y + 5;
		}

		canvas->blit(g_rectangle(0, 0, bufferInfo.width, bufferInfo.height));

		g_sleep(10);
	}
}
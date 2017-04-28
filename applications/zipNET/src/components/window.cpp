 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <components/window.hpp>
#include <zipNET.hpp>
#include <events/focus_event.hpp>
#include <events/key_event.hpp>
#include <events/mouse_event.hpp>
#include <mxuser/graphics/text/font_manager.hpp>
#include <mxuser/io/files/file_utils.hpp>
#include <mxuser/utils/environment.hpp>
#include <mxuser/ui/properties.hpp>
#include <cairo/cairo.h>
#include <components/cursor.hpp>
#include <math.h>

/**
 *
 */
Window_t::Window_t() : borderWidth(DEFAULT_BORDER_WIDTH), cornerSize(DEFAULT_CORNER_SIZE) 
{
	type = UI_COMPONENT_TYPE_WINDOW;

	visible = false;
	resizable = true;
	
	crossHovered = false;
	crossPressed = false;
	
	maximizeHovered = false;
	maximizePressed = false;
	isFullScreen = false;
	
	focused = false;
	shadowSize = 10;
	pathToLoad = false;
	shapeColor = RGB(230, 230, 230);

	style = Environment::getEnvVar("UISTYLE");

	Component_t::addChild(&label);
	Component_t::addChild(&panel);

	setMinimumSize(Dimension(100, 50));
}

/**
 *
 */
void Window_t::layout() 
{
	Rectangle bounds = getBounds();
	label.setBounds(Rectangle(15, 12, bounds.width - 30, 25));
	panel.setBounds(Rectangle(12, 40, bounds.width - 24, bounds.height - 52));
	crossBounds = Rectangle(bounds.width - 45, 10, 15, 15);
	maximizeBounds = Rectangle(bounds.width - 70, 13, 9, 9);
}

/**
 *
 */
void Window_t::addChild(Component_t *component) 
{
	panel.addChild(component);
}

void roundedRectangle(cairo_t *cr, double x, double y, double width, double height, double radius) 
{
	double degrees = M_PI / 180.0;
	cairo_new_sub_path(cr);
	cairo_arc(cr, x + width - radius, y + radius, radius, -90 * degrees, 0 * degrees);
	cairo_arc(cr, x + width - radius, y + height - radius, radius, 0 * degrees, 90 * degrees);
	cairo_arc(cr, x + radius, y + height - radius, radius, 90 * degrees, 180 * degrees);
	cairo_arc(cr, x + radius, y + radius, radius, 180 * degrees, 270 * degrees);
	cairo_close_path(cr);
}

/**
 *
 */
void Window_t::paint() 
{
	bounds = getBounds();
	cr = graphics.getContext();
	clearSurface();

	// draw shadow
	double shadowAlpha = 0.003;
	double shadowAlphaAdd = 0.003;
	double addIncr;
	if (focused) addIncr = 0.003; 
	else addIncr = 0.001;
	
	for (int i = 0; i < shadowSize + 4; i++) 
	{
		roundedRectangle(cr, i, i + 3, bounds.width - 2 * i, bounds.height - 2 * i, 10);
		cairo_set_source_rgba(cr, 0, 255, 0, shadowAlpha);
		cairo_stroke(cr);
		shadowAlpha = shadowAlpha + shadowAlphaAdd;
		shadowAlphaAdd = shadowAlphaAdd + addIncr;
	}

	// draw background
	double degrees = M_PI / 180.0;
	cairo_new_sub_path(cr);
	double radius = 5;
	cairo_arc(cr, shadowSize + radius, shadowSize + radius, radius, 180 * degrees, 270 * degrees);
	cairo_arc(cr, bounds.width - radius - shadowSize, shadowSize + radius, radius, -90 * degrees, 0 * degrees);
	cairo_line_to(cr, bounds.width - shadowSize, bounds.height - shadowSize);
	cairo_line_to(cr, shadowSize, bounds.height - shadowSize);
	cairo_close_path(cr);
	cairo_set_source_rgba(cr, ARGB_FR_FROM(shapeColor), ARGB_FG_FROM(shapeColor), ARGB_FB_FROM(shapeColor), focused ? 0.8 : 0.5);
	cairo_fill(cr);

	// draw cross
	cairo_set_source_rgba(cr, 200, 0, 0, 0.6);
	cairo_rectangle(cr, crossBounds.x - 10, crossBounds.y, crossBounds.width + 20, crossBounds.height);
	cairo_fill(cr);

	if (crossPressed && crossHovered) cairo_set_source_rgba(cr, 255, 0, 0, 255);
	else if (crossHovered) cairo_set_source_rgba(cr, 255, 0, 0, 255);
	else cairo_set_source_rgba(cr, 0, 0, 0, 255);
	
	int crossPadding = 3;
	cairo_set_line_width(cr, 1.5);
	cairo_move_to(cr, crossBounds.x + crossPadding, crossBounds.y + crossPadding);
	cairo_line_to(cr, crossBounds.x + crossBounds.width - crossPadding, crossBounds.y + crossBounds.height - crossPadding);
	cairo_stroke(cr);
	cairo_move_to(cr, crossBounds.x + crossBounds.width - crossPadding, crossBounds.y + crossPadding);
	cairo_line_to(cr, crossBounds.x + crossPadding, crossBounds.y + crossBounds.height - crossPadding);
	cairo_stroke(cr);

	if (resizable)
	{
		// draw maximize
		cairo_set_source_rgba(cr, 0, 200, 0, 0.6);
		cairo_rectangle(cr, maximizeBounds.x - 5, maximizeBounds.y - 3, maximizeBounds.width + 9, crossBounds.height);
		cairo_fill(cr);

		if (maximizeHovered && maximizePressed) cairo_set_source_rgba(cr, 0.1, 2, 0.1, 1);

		else if (maximizeHovered) cairo_set_source_rgba(cr, 0.1, 0.8, 0.1, 1);

		else cairo_set_source_rgba(cr, 0, 0, 0, 1);
		
		cairo_rectangle(cr, maximizeBounds.x, maximizeBounds.y, maximizeBounds.width, maximizeBounds.height);
		cairo_stroke(cr);
	}

	//create png if necessary
	if (pathToLoad)
	{
		pngSurface = graphics.getContext();

		cairo_set_source_surface(pngSurface, png, pngPosition.x, pngPosition.y);
		cairo_paint(pngSurface);
	}

}

/**
 *
 */
void Window_t::setPNG(std::string path, Point position)
{
	pathToLoad = true;
	pngPosition = position;

	png = cairo_image_surface_create_from_png(path.c_str());
	
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
void Window_t::PngAnimation(std::string path, Point PNGstartAnimation, Point PNGendAnimation, size_t sleep)
{
}

/**
 *
 */
void Window_t::setColor(ColorArgb color, ColorArgb tltColor)
{
	shapeColor = color;

	label.setFontColor(tltColor);

	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
void Window_t::handleBoundChange(Rectangle oldBounds) 
{
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
bool Window_t::handle(Event_t &event) 
{

	// Catch focus event
	FocusEvent_t *focusEvent = dynamic_cast<FocusEvent_t*>(&event);
	if (focusEvent) 
	{
		if (focusEvent->newFocusedComponent) this->focused = (this == focusEvent->newFocusedComponent) || focusEvent->newFocusedComponent->isChildOf(this);

		else this->focused = false;
		markFor(COMPONENT_REQUIREMENT_PAINT);
		return true;
	}

	// Let components handle input
	if (Component_t::handle(event)) return true;

	// Handle mouse events
	static Point pressPoint;
	static Rectangle pressBounds;
	static WindowResizeMode resizeMode;

	MouseEvent_t *mouseEvent = dynamic_cast<MouseEvent_t*>(&event);
	if (mouseEvent) 
	{
		Rectangle currentBounds = getBounds();

		if (mouseEvent->type == MOUSE_EVENT_MOVE) 
		{
			if (resizable) 
			{
				Point pos = mouseEvent->position;
				if ((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2) && (pos.y < cornerSize) && (pos.y > shadowSize - cornerSize / 2)) 
				{ // Top left corner
					Cursor_t::set("resize-nwes");
				} 

				else if ((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (pos.y < cornerSize) && (pos.y > shadowSize - cornerSize / 2)) 
				{ // Top right corner
					Cursor_t::set("resize-nesw");
				} 

				else if ((pos.x < shadowSize + cornerSize / 2) && (pos.x > shadowSize - cornerSize / 2) && (pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (pos.y < currentBounds.height - shadowSize + cornerSize / 2)) 
				{ // Bottom left corner
					Cursor_t::set("resize-nesw");
				} 

				else if ((pos.x > currentBounds.width - shadowSize - cornerSize / 2) && (pos.x < currentBounds.width - shadowSize + cornerSize / 2) && (pos.y > currentBounds.height - shadowSize - cornerSize / 2) && (pos.y < currentBounds.height - shadowSize + cornerSize / 2)) 
				{ // Bottom right corner
					Cursor_t::set("resize-nwes");
				} 

				else if (pos.y < shadowSize + borderWidth / 2 && pos.y > shadowSize - borderWidth / 2) 
				{	// Top edge
					Cursor_t::set("resize-ns");
				} 

				else if (pos.x < shadowSize + borderWidth / 2 && pos.x > shadowSize - borderWidth / 2) 
				{ // Left edge
					Cursor_t::set("resize-ew");
				} 

				else if ((pos.y > currentBounds.height - shadowSize - borderWidth / 2) && (pos.y < currentBounds.height - shadowSize + borderWidth / 2)) 
				{ // Bottom edge
					Cursor_t::set("resize-ns");
				} 

				else if ((pos.x > currentBounds.width - shadowSize - borderWidth / 2) && (pos.x < currentBounds.width - shadowSize + borderWidth / 2)) 
				{ // Right edge
					Cursor_t::set("resize-ew");
				} 

				else 
				{
					Cursor_t::set("default");
				}
			}

			// Cross
			if (crossBounds.contains(mouseEvent->position)) crossHovered = true;

			else crossHovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);

			if (maximizeBounds.contains(mouseEvent->position))maximizeHovered = true;

			else maximizeHovered = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);

		} 

		else if (mouseEvent->type == MOUSE_EVENT_DRAG) 
		{
			// Press on the cross
			if (crossPressed) 
			{
				crossHovered = crossBounds.contains(mouseEvent->position);
				markFor(COMPONENT_REQUIREMENT_PAINT);
			} 

			// press on maximize
			else if (resizable && maximizePressed)
			{
				maximizeHovered = maximizeBounds.contains(mouseEvent->position);
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}

			else 
			{

				// Window dragging/resizing
				Point newLocation = mouseEvent->screenPosition - pressPoint;

				// Calculate new bounds
				Rectangle newBounds = currentBounds;

				if (resizeMode == RESIZE_MODE_TOP_LEFT) 
				{
					newBounds.x = newLocation.x;
					newBounds.y = newLocation.y;
					newBounds.width = pressBounds.width + (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height + (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_TOP_RIGHT) 
				{
					newBounds.x = pressBounds.x;
					newBounds.y = newLocation.y;
					newBounds.width = pressBounds.width - (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height + (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_BOTTOM_LEFT) 
				{
					newBounds.x = newLocation.x;
					newBounds.y = pressBounds.y;
					newBounds.width = pressBounds.width + (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height - (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_BOTTOM_RIGHT) 
				{
					newBounds.x = pressBounds.x;
					newBounds.y = pressBounds.y;
					newBounds.width = pressBounds.width - (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height - (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_TOP) 
				{
					newBounds.x = pressBounds.x;
					newBounds.y = newLocation.y;
					newBounds.width = pressBounds.width;
					newBounds.height = pressBounds.height + (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_LEFT) 
				{
					newBounds.x = newLocation.x;
					newBounds.y = pressBounds.y;
					newBounds.width = pressBounds.width + (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height;
				} 

				else if (resizeMode == RESIZE_MODE_BOTTOM) 
				{
					newBounds.x = pressBounds.x;
					newBounds.y = pressBounds.y;
					newBounds.width = pressBounds.width;
					newBounds.height = pressBounds.height - (pressBounds.y - newLocation.y);
				} 

				else if (resizeMode == RESIZE_MODE_RIGHT) 
				{
					newBounds.x = pressBounds.x;
					newBounds.y = pressBounds.y;
					newBounds.width = pressBounds.width - (pressBounds.x - newLocation.x);
					newBounds.height = pressBounds.height;
				} 

				else if (resizeMode == RESIZE_MODE_MOVE) 
				{
					newBounds.x = newLocation.x;
					newBounds.y = newLocation.y;
				}

				// Apply bounds
				Rectangle appliedBounds = getBounds();
				if (newBounds.width > 50) 
				{
					appliedBounds.x = newBounds.x;
					appliedBounds.width = newBounds.width;
				}
				if (newBounds.height > 20) 
				{
					appliedBounds.y = newBounds.y;
					appliedBounds.height = newBounds.height;
				}
				this->setBounds(appliedBounds);
			}

		} 

		else if (mouseEvent->type == MOUSE_EVENT_PRESS) 
		{
			// Press on the cross
			if (crossBounds.contains(mouseEvent->position)) 
			{
				crossPressed = true;
				markFor(COMPONENT_REQUIREMENT_PAINT);
			} 

			// press on maximize
			else if (resizable && maximizeBounds.contains(mouseEvent->position))
			{
				maximizePressed = true;
				markFor(COMPONENT_REQUIREMENT_PAINT);
			}

			else 
			{
				// Window drag and resize
				pressPoint = mouseEvent->position;
				pressBounds = currentBounds;

				resizeMode = RESIZE_MODE_NONE;

				if (resizable) 
				{

					if ((pressPoint.x < shadowSize + cornerSize / 2) && (pressPoint.x > shadowSize - cornerSize / 2) && (pressPoint.y < cornerSize)
							&& (pressPoint.y > shadowSize - cornerSize / 2)) 
							{ // Corner resizing
						resizeMode = RESIZE_MODE_TOP_LEFT;
					} 

					else if ((pressPoint.x > currentBounds.width - shadowSize - cornerSize / 2)
							&& (pressPoint.x < currentBounds.width - shadowSize + cornerSize / 2) && (pressPoint.y < cornerSize)
							&& (pressPoint.y > shadowSize - cornerSize / 2)) 
					{
						resizeMode = RESIZE_MODE_TOP_RIGHT;
					} 

					else if ((pressPoint.x < shadowSize + cornerSize / 2) && (pressPoint.x > shadowSize - cornerSize / 2)
							&& (pressPoint.y > currentBounds.height - shadowSize - cornerSize / 2)
							&& (pressPoint.y < currentBounds.height - shadowSize + cornerSize / 2)) 
					{
						resizeMode = RESIZE_MODE_BOTTOM_LEFT;
					} 

					else if ((pressPoint.x > currentBounds.width - shadowSize - cornerSize / 2)
							&& (pressPoint.x < currentBounds.width - shadowSize + cornerSize / 2)
							&& (pressPoint.y > currentBounds.height - shadowSize - cornerSize / 2)
							&& (pressPoint.y < currentBounds.height - shadowSize + cornerSize / 2)) 
					{
						resizeMode = RESIZE_MODE_BOTTOM_RIGHT;

					} 

					else if (pressPoint.y < shadowSize + borderWidth / 2 && pressPoint.y > shadowSize - borderWidth / 2) 
					{	// Edge resizing
						resizeMode = RESIZE_MODE_TOP;
					} 

					else if (pressPoint.x < shadowSize + borderWidth / 2 && pressPoint.x > shadowSize - borderWidth / 2) 
					{
						resizeMode = RESIZE_MODE_LEFT;
					} 

					else if ((pressPoint.y > currentBounds.height - shadowSize - borderWidth / 2) && (pressPoint.y < currentBounds.height - shadowSize + borderWidth / 2)) 
					{
						resizeMode = RESIZE_MODE_BOTTOM;
					} 

					else if ((pressPoint.x > currentBounds.width - shadowSize - borderWidth / 2) && (pressPoint.x < currentBounds.width - shadowSize + borderWidth / 2)) 
					{
						resizeMode = RESIZE_MODE_RIGHT;
					}
				}

				if (resizeMode == RESIZE_MODE_NONE) 
				{
					if (pressPoint.y < 40) 
					{
						resizeMode = RESIZE_MODE_MOVE;
					}
				}
			}

		} 

		else if (mouseEvent->type == MOUSE_EVENT_LEAVE) Cursor_t::set("default");

		else if (mouseEvent->type == MOUSE_EVENT_DRAG_RELEASE) 
		{
			crossPressed = false;
			markFor(COMPONENT_REQUIREMENT_PAINT);

			if (crossBounds.contains(mouseEvent->position)) this->closeAll();

			else if (maximizeBounds.contains(mouseEvent->position))
			{
				if (!isFullScreen)
				{
					isFullScreen = true; 
					Dimension resolution = ZipNET::instance()->videoOutput->getResolution();

					Rectangle fullScreenBounds;
					if (style == "GNOME") fullScreenBounds = Rectangle(0, 30, resolution.width, resolution.height - 94);
					else if (style == "KDE") fullScreenBounds = Rectangle(0, 0, resolution.width, resolution.height - 30);
					
					this->setBounds(fullScreenBounds);
				}

				else 
				{
					isFullScreen = false;
					this->setBounds(SetInitialBounds);
				}
			}

		}

	}

	return true;
}

/**
 *
 */
void Window_t::closeAll() 
{
	EventListenerInfo_t info;
	if (getListener(UI_COMPONENT_EVENT_TYPE_CLOSE, info)) 
	{
		UiComponentCloseEvent postedEvent;
		postedEvent.header.type = UI_COMPONENT_EVENT_TYPE_CLOSE;
		postedEvent.header.componentID = info.componentID;
		SendMessage(info.targetThread, &postedEvent, sizeof(UiComponentCloseEvent));
	}
	setVisible(false);
}

/*
 *
 */
void Window_t::setFocus(bool focus)
{
	focused = focus;
	markFor(COMPONENT_REQUIREMENT_PAINT);
}

/**
 *
 */
bool Window_t::getNumericProperty(int property, uint32_t *out) 
{

	if (property == UI_PROPERTY_RESIZABLE) 
	{
		*out = resizable;
		return true;
	}

	return false;
}

/**
 *
 */
bool Window_t::setNumericProperty(int property, uint32_t value) 
{

	if (property == UI_PROPERTY_RESIZABLE) 
	{
		resizable = value;
		return true;
	}

	return false;
}

/**
 *
 */
void Window_t::setTitle(std::string title) 
{
	label.setTitle(title);
}

/**
 *
 */
std::string Window_t::getTitle() 
{
	return label.getTitle();
}

/**
 *
 */
void Window_t::setTitleFont(std::string fontName)
{
	label.setTitleFont(fontName);
}

/*
 *
 */
void Window_t::setTitleAlignment(TextAlignment alignment)
{
	label.setTitleAlignment(alignment);
}

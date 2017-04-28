#include <components/component.hpp>
#include <vector>

#include <mxuser/utils/logger.hpp>
#include <layout/grid_layout_manager.hpp>

#include <typeinfo>

/**
 *
 */
GridLayoutManager_t::GridLayoutManager_t(int columns, int rows) : columns(columns), rows(rows), padding(Insets(0, 0, 0, 0)), horizontalCellSpace(0), verticalCellSpace(0) 
{
}

/**
 *
 */
void GridLayoutManager_t::layout() 
{

	if (component == 0) 
	{
		return;
	}

	std::vector<Component_t*> &children = component->getChildren();

	Rectangle usedBounds = component->getBounds();
	usedBounds.x += padding.left;
	usedBounds.y += padding.top;
	usedBounds.width -= padding.left + padding.right;
	usedBounds.height -= padding.top + padding.bottom;

	int x = usedBounds.x;
	int y = usedBounds.y;
	int rowHeight = 0;

	int widthPerComponent = (columns > 0) ? (usedBounds.width / columns) : usedBounds.width;

	for (Component_t *c : children) 
	{

		int usedHeight = (rows > 0) ? (usedBounds.height / rows) : c->getPreferredSize().height;

		if (x + widthPerComponent > usedBounds.width) 
		{
			x = usedBounds.x;
			y += rowHeight;
			rowHeight = 0;
		}

		c->setBounds(Rectangle(x, y, widthPerComponent, usedHeight));
		x += widthPerComponent;

		if (usedHeight > rowHeight) 
		{
			rowHeight = usedHeight;
		}

	}
}

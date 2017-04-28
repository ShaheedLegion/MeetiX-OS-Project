#ifndef GRIDLAYOUTMANAGER_HPP_
#define GRIDLAYOUTMANAGER_HPP_

#include <layout/layout_manager.hpp>
#include <mxuser/graphics/metrics/insets.hpp>

/**
 *
 */
class GridLayoutManager_t: public LayoutManager_t 
{
private:
	int columns;
	int rows;
	Insets padding;
	int horizontalCellSpace;
	int verticalCellSpace;

public:
	GridLayoutManager_t(int columns, int rows);

	void setPadding(Insets _padding) 
	{
		padding = _padding;
	}

	Insets getPadding() const 
	{
		return padding;
	}

	void setHorizontalCellSpace(int _space) 
	{
		horizontalCellSpace = _space;
	}

	int getHorizontalCellSpace() const 
	{
		return horizontalCellSpace;
	}

	void setVerticalCellSpace(int _space) 
	{
		verticalCellSpace = _space;
	}

	int getVerticalCellSpace() const 
	{
		return verticalCellSpace;
	}

	virtual void layout();
};

#endif

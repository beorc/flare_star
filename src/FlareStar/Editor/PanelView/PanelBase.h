#pragma once

#include <MyGUI.h>
#include "PanelCell.h"

class PanelBase
{
public:
	PanelBase() : mPanelCell(0), mWidgetClient(null)
	{
	}

	virtual void initialiseCell(PanelCell * _cell)
	{
		mPanelCell = _cell;
		mWidgetClient = mPanelCell->getClient();
	}

	virtual void shutdownCell()
	{
		mPanelCell = 0;
		mWidgetClient = null;
	}

	virtual void changeWidth(int _width)
	{
	}

	virtual void show()
	{
		mPanelCell->mainWidget()->show();
		mPanelCell->eventUpdatePanel(mPanelCell);
	}

	virtual void hide()
	{
		mPanelCell->mainWidget()->hide();
		mPanelCell->eventUpdatePanel(mPanelCell);
	}

	inline PanelCell * getPanelCell() { return mPanelCell; }
	
protected:
	PanelCell * mPanelCell;
	MyGUI::WidgetPtr mWidgetClient;
};



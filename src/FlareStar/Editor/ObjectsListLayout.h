#pragma once

#include "PanelView/PanelCell.h"
#include "PanelView/PanelViewWindow.h"
#include "PanelObjectsList.h"

class ObjectsListLayout
{
public:
	ObjectsListLayout();
	virtual ~ObjectsListLayout(void);

	virtual bool Load();
	virtual void Close();
	void mousePressed(MyGUI::WidgetPtr _widget);

	void AddItem(Ogre::UTFString);
	void RemoveItem(Ogre::UTFString);
protected:
	PanelViewWindow View;	
	PanelObjectsList ObjectsList;
	
};

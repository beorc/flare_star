#pragma once

#include <MyGUI.h>
#include "PanelView\PanelBase.h"

class ObjectDescriptionLayout;

class PanelObjectsList : public PanelBase
{
public:

	PanelObjectsList();
	~PanelObjectsList();

	virtual void initialiseCell(PanelCell * _cell);
	virtual void shutdownCell();

	void AddItem(Ogre::UTFString);
	void RemoveItem(Ogre::UTFString);

private:	
	MyGUI::VectorWidgetPtr mItemsCheckBox;
	MyGUI::ButtonPtr ButtonAdd, ButtonRemove;
	ObjectDescriptionLayout *ObjectDescription;
	void mousePressed(MyGUI::WidgetPtr) ;
	void NewObject(MyGUI::WidgetPtr) ;
	void DeleteCurrentObject(MyGUI::WidgetPtr) ;
	int CurrentHeight, HeightStep, WidthStep, Height, Width;
	void UpdateItemsView();
	void RemoveSelection();
};



#include "..\StdAfx.h"
#include "ObjectsListLayout.h"
#include "..\GUISystem.h"
#include "..\CommonDeclarations.h"
#include "..\AApplication.h"

ObjectsListLayout::ObjectsListLayout()
{
}

ObjectsListLayout::~ObjectsListLayout(void)
{
}

bool ObjectsListLayout::Load()
{
	//MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	using namespace MyGUI;
	const IntSize & view = gui->getViewSize();
	const IntSize size(600, 300);

	View.initialise();

	//mPanelDirector.eventChangePanels = MyGUI::newDelegate(this, &DemoKeeper::notifyChangePanels);	
	View.addItem(&ObjectsList);
	

	return true;
}

void ObjectsListLayout::Close()
{
	View.shutdown();
}

void ObjectsListLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	
}

void ObjectsListLayout::AddItem(Ogre::UTFString name)
{
	ObjectsList.AddItem(name);
}

void ObjectsListLayout::RemoveItem(Ogre::UTFString name)
{
	ObjectsList.RemoveItem(name);
}


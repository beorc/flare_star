#include "StdAfx.h"
#include <OgreAL.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "GUIDescriptionLayout.h"
#include "GUISystem.h"
#include "Configuration.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "AApplication.h"


GUIDescriptionLayout::GUIDescriptionLayout(const char *name) : 
GUILayout(name)
{
}

GUIDescriptionLayout::~GUIDescriptionLayout(void)
{
}

bool GUIDescriptionLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("MessageButtonOK");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIDescriptionLayout::mousePressed);

	MyGUI::WidgetPtr widget = Widgets.front();
	
	widget->setPosition(gui->getViewWidth()/2-widget->getWidth()/2,gui->getViewHeight()/2-widget->getHeight()/2);

	return res;
}

void GUIDescriptionLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	Hide();
	if (BackLayout)
		BackLayout->Show();
}

void GUIDescriptionLayout::Show()
{	
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

	
	GUILayout::Show();
}


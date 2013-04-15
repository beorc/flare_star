#include "StdAfx.h"
#include <OgreAL.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "GuiNewGamelayout.h"
#include "GUISystem.h"
#include "Configuration.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "AApplication.h"


GUINewGameLayout::GUINewGameLayout(const char *name) : 
GUILayout(name)
{
}

GUINewGameLayout::~GUINewGameLayout(void)
{
}

bool GUINewGameLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("NewGameButtonOK");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUINewGameLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("NewGameButtonCancel");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUINewGameLayout::mousePressed);

	/*button = gui->findWidget<MyGUI::Button>("ButtonOptions");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUINewGameLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonCredits");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUINewGameLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonQuit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUINewGameLayout::mousePressed);*/

	MyGUI::WidgetPtr widget = Widgets.front();
	
	widget->setPosition(gui->getViewWidth()/2-widget->getWidth()/2,gui->getViewHeight()/2-widget->getHeight()/2);

	return res;
}

void GUINewGameLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	Ogre::UTFString wtext;
	std::string text;	
	if (_widget->getName()=="NewGameButtonOK")
	{
		size_t position=0;
		MyGUI::EditPtr widget;

		MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

		widget = gui->findWidget<MyGUI::Edit>("NewGameEditName");			
		wtext = widget->getOnlyText();
        text = AAUtilities::WStringToString(wtext.asWStr());
		//ProfileName = AAUtilities::StringCopy(text.c_str());
		std::string wpath="profiles/";
		wpath+=text;
		BOOL copy_res = CopyFile("profiles/default.xml", wpath.c_str(), TRUE);

		if (!copy_res)
		{
			return;
		} else
		{
			Hide();
			CommonDeclarations::GetApplication()->ResetGame();
			CommonDeclarations::SetCurrentProfileName(text.c_str());
			CommonDeclarations::GetApplication()->StartGame();
			return;
		}
	}
	else
		if (_widget->getName()=="NewGameButtonCancel")
		{		
			Hide();
		}
	if (BackLayout)
		BackLayout->Show();
}

void GUINewGameLayout::Show(GUILayout *back_layout)
{	
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

	
	GUILayout::Show(back_layout);
}


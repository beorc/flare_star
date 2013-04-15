#include "StdAfx.h"
#include <OgreAL.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "GuiLoadGameLayout.h"
#include "GUISystem.h"
#include "Configuration.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "AApplication.h"


GUILoadGameLayout::GUILoadGameLayout(const char *name) : 
GUILayout(name)
{
}

GUILoadGameLayout::~GUILoadGameLayout(void)
{
}

bool GUILoadGameLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("LoadGameButtonCancel");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::mousePressed);
	
	button = gui->findWidget<MyGUI::Button>("LoadGameButton1");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::LoadGameHandler);
	GameButtons.push_back(button);

	button = gui->findWidget<MyGUI::Button>("LoadGameButton2");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::LoadGameHandler);
	GameButtons.push_back(button);
	
	button = gui->findWidget<MyGUI::Button>("LoadGameButton3");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::LoadGameHandler);
	GameButtons.push_back(button);

	button = gui->findWidget<MyGUI::Button>("LoadGameButton4");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::LoadGameHandler);
	GameButtons.push_back(button);

	button = gui->findWidget<MyGUI::Button>("LoadGameButton5");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUILoadGameLayout::LoadGameHandler);
	GameButtons.push_back(button);

	MyGUI::WidgetPtr widget = Widgets.front();
	
	//widget->setPosition(gui->getViewWidth()/2-widget->getWidth()/2,gui->getViewHeight()/2-widget->getHeight()/2);

	return res;
}

void GUILoadGameLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	Hide();
	if (BackLayout)
		BackLayout->Show();
}

void GUILoadGameLayout::Show(GUILayout *back_layout)
{	
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	char count=0;
	
	hFind = FindFirstFile(_T("profiles/*"), &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		
		
	} 
	else 
	{
		do {
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				continue;
			if (_tcscmp(FindFileData.cFileName,_T("default.xml"))!=0)
			{
				GameButtons[count]->setCaption(FindFileData.cFileName);
				if (++count>4)
					break;
			}
		} while (FindNextFile(hFind, &FindFileData) != 0);
		
		FindClose(hFind);		
	}


	
	GUILayout::Show(back_layout);
}

void GUILoadGameLayout::LoadGameHandler(MyGUI::WidgetPtr _widget)
{
    Ogre::UTFString caption;
    std::string text;
    std::vector<MyGUI::ButtonPtr>::iterator iPos=GameButtons.begin(), iEnd=GameButtons.end();
    for (;iPos!=iEnd;++iPos)
    {
        if (*iPos==_widget)
        {
            caption = _widget->getCaption();
            text = AAUtilities::WStringToString(caption.asWStr());
			if (text=="Empty") 
			{
				return;
			}
            Hide();			
            CommonDeclarations::GetApplication()->ResetGame();
            CommonDeclarations::SetCurrentProfileName(text.c_str());
            CommonDeclarations::GetApplication()->StartGame();
        }
    }
}


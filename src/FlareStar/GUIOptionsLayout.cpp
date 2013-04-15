#include "StdAfx.h"
#include <OgreAL.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "GuiOptionslayout.h"
#include "GUISystem.h"
#include "Configuration.h"
#include "CommonDeclarations.h"
#include "Profile.h"


GUIOptionsLayout::GUIOptionsLayout(const char *name) : 
GUILayout(name)
{
}

GUIOptionsLayout::~GUIOptionsLayout(void)
{
}

bool GUIOptionsLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("OptionsButtonOK");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("OptionsButtonCancel");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);

	/*button = gui->findWidget<MyGUI::Button>("ButtonOptions");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonCredits");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonQuit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);*/

	MyGUI::WidgetPtr widget = Widgets.front();
	
	widget->setPosition(gui->getViewWidth()/2-widget->getWidth()/2,gui->getViewHeight()/2-widget->getHeight()/2);

	//MyGUI::VectorWidgetPtr::iterator iPos = Widgets.begin(), iEnd=Widgets.end();	
	//for (;iPos!=iEnd;++iPos)
	//{
	//	widget = *iPos;
	//	//widget->getChilds();
	//	if (widget->getWidgetType()=="Button")
	//	{
	//		MyGUI::ButtonPtr button = dynamic_cast<MyGUI::ButtonPtr>(widget);
	//		button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIOptionsLayout::mousePressed);
	//	}

	//}

	return res;
}

void GUIOptionsLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{
	Hide();
	if (_widget->getName()=="OptionsButtonOK")
	{
		size_t position=0;
		MyGUI::HScrollPtr scroll;

		MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

		scroll = gui->findWidget<MyGUI::HScroll>("OptionsSliderSound");
		position = scroll->getScrollPosition();		
		Configuration::GetInstance()->SetEffectsVolume(position);
		CommonDeclarations::GetSoundManager()->setGainForAllSounds(position/100.f);

		scroll = gui->findWidget<MyGUI::HScroll>("OptionsSliderMusic");
		position = scroll->getScrollPosition();
		Configuration::GetInstance()->SetMusicVolume(position);
		CommonDeclarations::SetMusicGain(position/100.f);

		AAProfile *profile = CommonDeclarations::GetProfile();
		if (profile)
			profile->SaveConfiguration();
		//Configuration::GetInstance()->Save();		
	}
	else
		if (_widget->getName()=="OptionsButtonCancel")
		{			
		}
	if (BackLayout)
		BackLayout->Show();
	//GUISystem::GetInstance()->GetMainMenu()->Show();
}

void GUIOptionsLayout::Show(GUILayout *back_layout)
{	
	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();

	MyGUI::HScrollPtr scroll;
	scroll = gui->findWidget<MyGUI::HScroll>("OptionsSliderMusic");
	scroll->setScrollPosition(Configuration::GetInstance()->GetMusicVolume());
	
	scroll = gui->findWidget<MyGUI::HScroll>("OptionsSliderSound");
	scroll->setScrollPosition(Configuration::GetInstance()->GetEffectsVolume());	

	GUILayout::Show(back_layout);
}


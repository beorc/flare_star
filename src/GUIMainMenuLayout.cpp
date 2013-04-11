#include "StdAfx.h"
#include ".\guimainmenulayout.h"
#include "GUISystem.h"
#include "CommonDeclarations.h"
#include "AApplication.h"

GUIMainMenuLayout::GUIMainMenuLayout(const char *name) : 
GUILayout(name)
{
}

GUIMainMenuLayout::~GUIMainMenuLayout(void)
{
}

bool GUIMainMenuLayout::Load()
{
	bool res = GUILayout::Load();
	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("ButtonNewGame");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonContinue");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonOptions");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonCredits");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonQuit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("ButtonResume");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);

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
	//		button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIMainMenuLayout::mousePressed);
	//	}

	//}

	return res;
}

void GUIMainMenuLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	if (_widget->getName()=="ButtonQuit")
		GUISystem::GetInstance()->RequestShutDown();
	else
	if (_widget->getName()=="ButtonOptions")
	{
		Hide();
		GUISystem::GetInstance()->GetOptions()->Show(this);
	}
	else
		if (_widget->getName()=="ButtonResume")
		{
			//CommonDeclarations::GetApplication()->SwitchListener();
			CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GAME);
		} else
			if (_widget->getName()=="ButtonNewGame")
			{
				Hide();
				GUISystem::GetInstance()->NewGameHandler(this);
			} else
				if (_widget->getName()=="ButtonContinue")
				{
					Hide();
					GUISystem::GetInstance()->LoadGameHandler(this);
				}
}


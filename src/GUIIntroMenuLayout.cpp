#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "guiintromenulayout.h"
#include "GUISystem.h"
#include "CommonDeclarations.h"
#include "AApplication.h"

GUIIntroMenuLayout::GUIIntroMenuLayout(const char *name) : 
GUILayout(name)
{
}

GUIIntroMenuLayout::~GUIIntroMenuLayout(void)
{    
}

bool GUIIntroMenuLayout::Load()
{
	bool res = GUILayout::Load();

    //IntroMenuBackground = new Ogre::Rectangle2D(TRUE);
    //IntroMenuBackground->setMaterial("GUI/Radar");	    
    ////rect->getMaterial()->setDepthCheckEnabled(false);
    ////rect->getMaterial()->setDepthWriteEnabled(false);	
    ////rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY-2);
    //CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(IntroMenuBackground); 
    //IntroMenuBackground->setCorners(-1, 1, 1, -1);
    //IntroMenuBackground->setVisible(false);
    	
	MyGUI::ButtonPtr button;

	MyGUI::Gui *gui = GUISystem::GetInstance()->GetGui();
	
	button = gui->findWidget<MyGUI::Button>("IntroButtonNewGame");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("IntroButtonContinue");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("IntroButtonOptions");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("IntroButtonCredits");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);

	button = gui->findWidget<MyGUI::Button>("IntroButtonQuit");
	button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);

	MyGUI::WidgetPtr widget = Widgets.front();
		
	//widget->setPosition(gui->getViewWidth()/2-widget->getWidth()/2,gui->getViewHeight()/2-widget->getHeight()/2);
    
	//MyGUI::VectorWidgetPtr::iterator iPos = Widgets.begin(), iEnd=Widgets.end();	
	//for (;iPos!=iEnd;++iPos)
	//{
	//	widget = *iPos;
	//	//widget->getChilds();
	//	if (widget->getWidgetType()=="Button")
	//	{
	//		MyGUI::ButtonPtr button = dynamic_cast<MyGUI::ButtonPtr>(widget);
	//		button->eventMouseButtonClick = MyGUI::newDelegate(this, &GUIIntroMenuLayout::mousePressed);
	//	}

	//}

	return res;
}

void GUIIntroMenuLayout::mousePressed(MyGUI::WidgetPtr _widget) 
{	
	if (_widget->getName()=="IntroButtonQuit")
		GUISystem::GetInstance()->RequestShutDown();
	else
	if (_widget->getName()=="IntroButtonOptions")
	{
		Hide();
		GUISystem::GetInstance()->GetOptions()->Show(this);
	} else
		if (_widget->getName()=="IntroButtonNewGame")
		{
			Hide();
			GUISystem::GetInstance()->NewGameHandler(this);
		} else
			if (_widget->getName()=="IntroButtonContinue")
			{
				Hide();
				GUISystem::GetInstance()->LoadGameHandler(this);
			}	
}

void GUIIntroMenuLayout::Hide()
{
	GUILayout::Hide();
	//CommonDeclarations::PlayIntroMusic(false);
}

void GUIIntroMenuLayout::Show(GUILayout *back_layout)
{
	GUILayout::Show(back_layout);
	//CommonDeclarations::PlayIntroMusic(true);
}


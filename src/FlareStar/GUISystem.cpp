#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\guisystem.h"
#include "CommonDeclarations.h"
#include "AApplication.h"
#include "GUIIntroMenuLayout.h"
#include "GUIMainMenuLayout.h"
#include "GUIRadarLayout.h"
#include "GuiOptionslayout.h"
#include "GUINewGameLayout.h"
#include "GUILoadGameLayout.h"
#include "AACamera.h"

//todo
// HUD

//GUISystem *GUISystem::Instance = NULL;

GUISystem::GUISystem(void) :
mMouse(NULL),
mKeyboard(NULL),
mInputManager(NULL),
mWindow(NULL),
IntroMenuBackground(NULL),
mShutdownRequested(false)
{
	//Instance = new GUISystem;
}

GUISystem::~GUISystem(void)
{
	Reset();
}

GUISystem *GUISystem::GetInstance()
{
	static GUISystem *Instance = new GUISystem;	
    return Instance;
}

bool GUISystem::Init(Ogre::RenderWindow* window)
{
    GUI.reset(new MyGUI::Gui);
    GUI->initialise(window, "core.xml", GUI_RESOURCE_GROUP);
	GUI->hidePointer();

	using namespace OIS;
	ParamList pl;	
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow = window;
	window->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << (unsigned)windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, true ));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, true ));

	//unsigned int width, height, depth;
	//int left, top;
	//window->getMetrics(width, height, depth, left, top);

	////Set Mouse Region.. if window resizes, we should alter this to reflect as well
	//const MouseState &ms = mMouse->getMouseState();
	//ms.width = width;
	//ms.height = height;

	////mWindow = window;

	////Register as a Window listener
	//WindowEventUtilities::addWindowEventListener(mWindow, this);

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	IntroMenu.reset(new GUIIntroMenuLayout("IntroMenu.layout"));
	IntroMenu->Load();

	MainMenu.reset(new GUIMainMenuLayout("MainMenu.layout"));
	MainMenu->Load();

	Radar.reset(new GUIRadarLayout("radar.layout"));
	//Radar->Load();
	//Radar->Show();

	Options.reset(new GUIOptionsLayout("options.layout"));
	Options->Load();

	NewGame.reset(new GUINewGameLayout("NewGame.layout"));
	NewGame->Load();

	LoadGame.reset(new GUILoadGameLayout("LoadGame.layout"));
	LoadGame->Load();

	//IntroMenuBackground = new Ogre::Rectangle2D(TRUE);
	//IntroMenuBackground->setMaterial("wallpaper");	    
	////rect->getMaterial()->setDepthCheckEnabled(false);
	////IntroMenuBackground->getMaterial()->setDepthWriteEnabled(false);	
	//IntroMenuBackground->setRenderQueueGroup(Ogre::RENDER_QUEUE_BACKGROUND);
	//CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(IntroMenuBackground); 
	//IntroMenuBackground->setCorners(-1, 1, 1, -1);
	//IntroMenuBackground->setVisible(true);

	windowResized(mWindow);

    return true;
}

void GUISystem::Reset()
{
	if(mInputManager)
	{
		mInputManager->destroyInputObject(mMouse);
		mInputManager->destroyInputObject(mKeyboard);
		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = NULL;			
	}
	//Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);

	if (NULL!=GUI.get())
	{
		GUI->shutdown();
		GUI.reset();
	}
	if (NULL!=Radar.get())
	{
		Radar.reset();
	}

	IntroMenu.reset();
	MainMenu.reset();
	Radar.reset();
	Options.reset();
	NewGame.reset();
	LoadGame.reset();
}


void GUISystem::Close()
{
	delete GetInstance();
}

//Adjust mouse clipping area
void GUISystem::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Override frameStarted event to process that (don't care about frameEnded)
bool GUISystem::frameStarted(const Ogre::FrameEvent& evt)
{
	clock_t ticks = evt.timeSinceLastFrame*1000, wait_ticks=0;

	while (ticks<TARGET_UPDATE_INTERVAL_MS)
	{
		wait_ticks = clock();
		Sleep(1);		
		ticks += clock()-wait_ticks;		
	}

	if(mWindow->isClosed())	return false;

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	GUI->injectFrameEntered(evt.timeSinceLastFrame);

	return true;
}

// Override frameEnded event 
bool GUISystem::frameEnded(const Ogre::FrameEvent& evt)
{
	if (mShutdownRequested)
		return false;
	return true;
}

/// Tell the frame listener to exit at the end of the next frame
void GUISystem::requestShutdown(void)
{	
}

//----------------------------------------------------------------//
bool GUISystem::mouseMoved( const OIS::MouseEvent &arg )
{
	GUI->injectMouseMove(arg);
	return true;
}

//----------------------------------------------------------------//
bool GUISystem::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	GUI->injectMousePress(arg, id);
	return true;
}

//----------------------------------------------------------------//
bool GUISystem::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	GUI->injectMouseRelease(arg, id);
	return true;
}

//----------------------------------------------------------------//
//----------------------------------------------------------------//
bool GUISystem::keyPressed( const OIS::KeyEvent &arg )
{
	/*if (arg.key==OIS::KC_ESCAPE)
		CommonDeclarations::GetApplication()->SwitchListener();
	else*/
		GUI->injectKeyPress(arg);

	
	return true;
}

//----------------------------------------------------------------//
bool GUISystem::keyReleased( const OIS::KeyEvent &arg )
{
	GUI->injectKeyRelease(arg);
	return true;
}

void GUISystem::Show()
{
    /*int x=CommonDeclarations::GetCamera()->GetOgreCamera()->getViewport()->getActualWidth()/2,
        y=CommonDeclarations::GetCamera()->GetOgreCamera()->getViewport()->getActualHeight()/2;*/
	const OIS::MouseState &ms = mMouse->getMouseState();
	int x=ms.width/2,
		y=ms.height/2;
	mMouse->setMousePosition(x,y);

    GUI->setPointerPosition(x, y);	
	GUI->showPointer();   
	//MainMenu->Show();
	//if (IntroMenuBackground)
	//	IntroMenuBackground->setVisible(true);
}

void GUISystem::Hide()
{
	GUI->hidePointer();	
	MainMenu->Hide();
	IntroMenu->Hide();
	MainMenu->Hide();
	//Radar->Hide();
	Options->Hide();
}

void GUISystem::RequestShutDown()
{
	mShutdownRequested=true;
}

MyGUI::Gui *GUISystem::GetGui()
{
	return GUI.get();
}

GUILayout *GUISystem::GetIntroMenu()
{
	return IntroMenu.get();
}

GUILayout *GUISystem::GetMainMenu()
{
	return MainMenu.get();
}

GUILayout *GUISystem::GetRadar()
{
    return Radar.get();
}

GUILayout *GUISystem::GetOptions()
{
	return Options.get();
}

void GUISystem::NewGameHandler(GUILayout *back_layout)
{
	NewGame->Show(back_layout);
}

void GUISystem::LoadGameHandler(GUILayout *back_layout)
{
	LoadGame->Show(back_layout);
}

GUILayout *GUISystem::GetNewGameMenu()
{
	return NewGame.get();
}

void GUISystem::SetHudVisible(bool on)
{
	if (Radar.get())
	{
		if (on)
			Radar->Show();
		else
			Radar->Hide();
	}
}

void GUISystem::ShowDescription(IDescriptable *descr)
{	
	assert(descr);
	if (NULL==descr)
		return;
	
	//TiXmlElement* xml = descr->GetDescription();
	const char* str = descr->GetIconName();
	str = descr->GetHeader();
	str = descr->GetText();
}

void GUISystem::HideDescription()
{
}

void GUISystem::FlushInput()
{
	mKeyboard->capture();
	mMouse->capture();
}


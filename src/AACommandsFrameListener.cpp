#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AACommandsFrameListener.h"
#include "CommonDeclarations.h"
#include "IFrameHandler.h"
#include "AApplication.h"
#include "GUISystem.h"
#include "GUILayout.h"
#define OIS_DYNAMIC_LIB
#include <OIS.h>

AACommandsFrameListener::AACommandsFrameListener(void) :
mMouse(NULL),
mKeyboard(NULL),
mInputManager(NULL),
mWindow(NULL),
mShutdownRequested(false),
CurrentHandler(NULL),
LastFrameTime(0),
Blocked(false)
{
}

AACommandsFrameListener::~AACommandsFrameListener(void)
{
}

void AACommandsFrameListener::SetBlocked(bool on)
{
	Blocked=on;
}

AACommandsFrameListener *AACommandsFrameListener::GetInstance()
{
	static AACommandsFrameListener *Instance = new AACommandsFrameListener;	
    return Instance;
}

bool AACommandsFrameListener::Init(Ogre::RenderWindow* window)
{
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

	//mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	windowResized(mWindow);

    return true;
}

void AACommandsFrameListener::Close()
{
	delete GetInstance();
}

//Adjust mouse clipping area
void AACommandsFrameListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Override frameStarted event to process that (don't care about frameEnded)
bool AACommandsFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	if (0==LastFrameTime)
		LastFrameTime = clock();

	clock_t ticks = clock()-LastFrameTime, wait_ticks=0; //evt.timeSinceLastFrame*1000

	LastFrameTime = clock();

	//while (ticks<TARGET_UPDATE_INTERVAL_MS)
	//{
	//	wait_ticks = clock();
	//	Sleep(1);		
	//	ticks += clock()-wait_ticks;		
	//}

	//if(mWindow->isClosed())	return false;

	//Need to capture/update each device	
	mKeyboard->capture();
	//mMouse->capture();

    //if (CurrentHandler)
    //    CurrentHandler->Step(ticks);

	return true;
}

// Override frameEnded event 
bool AACommandsFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	return true;
}

/// Tell the frame listener to exit at the end of the next frame
void AACommandsFrameListener::requestShutdown(void)
{	
}

//----------------------------------------------------------------//
bool AACommandsFrameListener::mouseMoved( const OIS::MouseEvent &arg )
{
	//GUI->injectMouseMove(arg);
	return true;
}

//----------------------------------------------------------------//
bool AACommandsFrameListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	//GUI->injectMousePress(arg, id)
	//if (CurrentHandler)
	//{
	//	return CurrentHandler->MousePressed(arg, id);
	//};
	return true;
}

//----------------------------------------------------------------//
bool AACommandsFrameListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	//GUI->injectMouseRelease(arg, id);
	return true;
}

//----------------------------------------------------------------//
//----------------------------------------------------------------//
bool AACommandsFrameListener::keyPressed( const OIS::KeyEvent &arg )
{	
	if( arg.key == OIS::KC_ESCAPE )
	{		
		CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GUI);
		GUISystem::GetInstance()->GetMainMenu()->Show();
		GUISystem::GetInstance()->Show();
		return true;		
	}

	if (Blocked)
		return true;

	if (CurrentHandler)
	{
		return CurrentHandler->KeyPressed(arg);
	}

	return true;
}

//----------------------------------------------------------------//
bool AACommandsFrameListener::keyReleased( const OIS::KeyEvent &arg )
{
	//GUI->injectKeyRelease(arg);
	return true;
}

void AACommandsFrameListener::RequestShutDown()
{
	mShutdownRequested=true;
}

void AACommandsFrameListener::SetCurrentHandler(IFrameHandler *handler)
{
    if (CurrentHandler)
        delete CurrentHandler;
	ResetBuffer();
	if (CurrentHandler!=NULL)
	{		
		CurrentHandler = NULL;				
	}	
    CurrentHandler = handler;
	LastFrameTime=0;
}

IFrameHandler *AACommandsFrameListener::GetCurrentHandler() const
{
	return CurrentHandler;
}

void AACommandsFrameListener::SetLastFrameTime(clock_t time)
{
	LastFrameTime = time;
}

void AACommandsFrameListener::ResetBuffer()
{
	Blocked = true;
	mKeyboard->capture();
	mMouse->capture();
	Blocked = false;
}


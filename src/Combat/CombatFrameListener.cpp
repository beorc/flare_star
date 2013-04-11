#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "CombatFrameListener.h"
#include "CommonDeclarations.h"
#include "IFrameHandler.h"

CombatFrameListener::CombatFrameListener(void) :
mMouse(NULL),
mKeyboard(NULL),
mInputManager(NULL),
mWindow(NULL),
mShutdownRequested(false),
CurrentHandler(NULL),
LastFrameTime(0)
{
}

CombatFrameListener::~CombatFrameListener(void)
{
}

CombatFrameListener *CombatFrameListener::GetInstance()
{
	static CombatFrameListener *Instance = new CombatFrameListener;	
    return Instance;
}

bool CombatFrameListener::Init(Ogre::RenderWindow* window)
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

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	windowResized(mWindow);

    return true;
}

void CombatFrameListener::Close()
{
	delete GetInstance();
}

//Adjust mouse clipping area
void CombatFrameListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Override frameStarted event to process that (don't care about frameEnded)
bool CombatFrameListener::frameStarted(const Ogre::FrameEvent& evt)
{
	if (0==LastFrameTime)
		LastFrameTime = clock();

	clock_t ticks = clock()-LastFrameTime, wait_ticks=0; //evt.timeSinceLastFrame*1000

	LastFrameTime = clock();

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

    if (CurrentHandler)
        CurrentHandler->Step(ticks);

	//GUI->injectFrameEntered(evt.timeSinceLastFrame);

	return true;
}

// Override frameEnded event 
bool CombatFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	if (mShutdownRequested)
		return false;
	return true;
}

/// Tell the frame listener to exit at the end of the next frame
void CombatFrameListener::requestShutdown(void)
{	
}

//----------------------------------------------------------------//
bool CombatFrameListener::mouseMoved( const OIS::MouseEvent &arg )
{
	//GUI->injectMouseMove(arg);
	return true;
}

//----------------------------------------------------------------//
bool CombatFrameListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	//GUI->injectMousePress(arg, id)
	if (CurrentHandler)
	{
		return CurrentHandler->MousePressed(arg, id);
	};
	return true;
}

//----------------------------------------------------------------//
bool CombatFrameListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	//GUI->injectMouseRelease(arg, id);
	return true;
}

//----------------------------------------------------------------//
//----------------------------------------------------------------//
bool CombatFrameListener::keyPressed( const OIS::KeyEvent &arg )
{
	//GUI->injectKeyPress(arg);
	if (CurrentHandler)
	{
		return CurrentHandler->KeyPressed(arg);
	}
	return true;
}

//----------------------------------------------------------------//
bool CombatFrameListener::keyReleased( const OIS::KeyEvent &arg )
{
	//GUI->injectKeyRelease(arg);
	return true;
}

void CombatFrameListener::RequestShutDown()
{
	mShutdownRequested=true;
}

void CombatFrameListener::SetCurrentHandler(IFrameHandler *handler)
{
    if (CurrentHandler)
        delete CurrentHandler;
	if (CurrentHandler!=NULL)
	{
		CurrentHandler = NULL;
		mKeyboard->capture();
		mMouse->capture();
	}	
    CurrentHandler = handler;
	LastFrameTime=0;
}

IFrameHandler *CombatFrameListener::GetCurrentHandler() const
{
	return CurrentHandler;
}

void CombatFrameListener::SetLastFrameTime(clock_t time)
{
	LastFrameTime = time;
}


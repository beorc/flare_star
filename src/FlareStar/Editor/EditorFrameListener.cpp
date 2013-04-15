#include "..\StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "EditorFrameListener.h"
#include "..\CommonDeclarations.h"
#include "..\IFrameHandler.h"
#include "..\GUISystem.h"
#include "..\AApplication.h"
#include "EditorFrameHandler.h"

EditorFrameListener::EditorFrameListener(void) :
mMouse(NULL),
mKeyboard(NULL),
mInputManager(NULL),
mWindow(NULL),
mShutdownRequested(false),
CurrentHandler(NULL),
LastFrameTime(0)
{
}

EditorFrameListener::~EditorFrameListener(void)
{
}

EditorFrameListener *EditorFrameListener::GetInstance()
{
	static EditorFrameListener *Instance = new EditorFrameListener;	
    return Instance;
}

bool EditorFrameListener::Init(Ogre::RenderWindow* window)
{
	/*GUI = GUISystem::GetInstance()->GetGui();
	GUI->hidePointer();*/

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

void EditorFrameListener::Close()
{
	delete GetInstance();
}

//void EditorFrameListener::Show()
//{
//	const OIS::MouseState &ms = mMouse->getMouseState();
//	int x=ms.width/2,
//		y=ms.height/2;
//	mMouse->setMousePosition(x,y);
//    GUI->setPointerPosition(x, y);	
//	GUI->showPointer();   
//}
//
//void EditorFrameListener::Hide()
//{
//	GUI->hidePointer();	
//}

//Adjust mouse clipping area
void EditorFrameListener::windowResized(Ogre::RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

// Override frameStarted event to process that (don't care about frameEnded)
bool EditorFrameListener::frameStarted(const Ogre::FrameEvent& evt)
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

	GUISystem::GetInstance()->GetGui()->injectFrameEntered(evt.timeSinceLastFrame);

	return true;
}

// Override frameEnded event 
bool EditorFrameListener::frameEnded(const Ogre::FrameEvent& evt)
{
	if (mShutdownRequested)
	{
		mShutdownRequested = false;
		SetCurrentHandler(NULL);
		CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GAME);

		//return false;
	}
	return true;
}

//----------------------------------------------------------------//
bool EditorFrameListener::mouseMoved( const OIS::MouseEvent &arg )
{	
	GUISystem::GetInstance()->GetGui()->injectMouseMove(arg);
	if (CurrentHandler)
	{
		return CurrentHandler->MouseMoved(arg);
	}
	return true;
}

//----------------------------------------------------------------//
bool EditorFrameListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	GUISystem::GetInstance()->GetGui()->injectMousePress(arg, id);
	if (CurrentHandler)
	{
		return CurrentHandler->MousePressed(arg, id);
	}
	return true;
}

//----------------------------------------------------------------//
bool EditorFrameListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	GUISystem::GetInstance()->GetGui()->injectMouseRelease(arg, id);
	if (CurrentHandler)
	{
		return CurrentHandler->MouseReleased(arg, id);
	}
	return true;
}

//----------------------------------------------------------------//
//----------------------------------------------------------------//
bool EditorFrameListener::keyPressed( const OIS::KeyEvent &arg )
{
	GUISystem::GetInstance()->GetGui()->injectKeyPress(arg);
	if (CurrentHandler)
	{
		return CurrentHandler->KeyPressed(arg);
	}
	return true;
}

//----------------------------------------------------------------//
bool EditorFrameListener::keyReleased( const OIS::KeyEvent &arg )
{
	GUISystem::GetInstance()->GetGui()->injectKeyRelease(arg);
	if (CurrentHandler)
	{
		return CurrentHandler->KeyReleased(arg);
	}
	
	return true;
}

void EditorFrameListener::RequestShutDown()
{
	mShutdownRequested=true;
}

void EditorFrameListener::SetCurrentHandler(EditorFrameHandler *handler)
{
    if (CurrentHandler!=NULL)
	{
		delete CurrentHandler;
		CurrentHandler = NULL;
		mKeyboard->capture();
		mMouse->capture();
	}	
    CurrentHandler = handler;
	LastFrameTime=0;
}

EditorFrameHandler *EditorFrameListener::GetCurrentHandler() const
{
	return CurrentHandler;
}

void EditorFrameListener::SetLastFrameTime(clock_t time)
{
	LastFrameTime = time;
}

OIS::Mouse *EditorFrameListener::GetMouse() const
{
	return mMouse;
}





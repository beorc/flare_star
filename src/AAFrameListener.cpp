#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AAFrameListener.h"
#include "Core.h"
#include "Player.h"
#include "AACamera.h"
#include "CommonDeclarations.h"
#include "AApplication.h"
#include "GUISystem.h"
#include "GUILayout.h"

AAFrameListener::AAFrameListener(RenderWindow* win, AACamera* cam, bool bufferedKeys, bool bufferedMouse) :
PlayersCamera(cam),
mWindow(win),
mStatsOn(false),
mNumScreenShots(0),
mTimeUntilNextToggle(0),
mFiltering(TFO_BILINEAR),
mAniso(1),
mSceneDetailIndex(0),
mDebugOverlay(0),
mInputManager(0),
mMouse(0),
mKeyboard(0),
mShutdownRequested(false),
CoreInstance(Core::GetInstance()),
Blocked(false)
{	
	using namespace OIS;
	ParamList pl;	
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	win->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << (unsigned)windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = InputManager::createInputSystem( pl );

	//Create all devices (We only catch joystick exceptions here, as, most people have Key/Mouse)
	mKeyboard = static_cast<Keyboard*>(mInputManager->createInputObject( OISKeyboard, bufferedKeys ));
	mMouse = static_cast<Mouse*>(mInputManager->createInputObject( OISMouse, bufferedMouse ));

	unsigned int width, height, depth;
	int left, top;
	win->getMetrics(width, height, depth, left, top);

	//Set Mouse Region.. if window resizes, we should alter this to reflect as well
	const MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
	
	mWindow = win;
	//mStatsOn = true;
	//mNumScreenShots = 0;
	//mTimeUntilNextToggle = 0;

	//Set initial mouse clipping size
	windowResized(mWindow);
	
	//showDebugOverlay(true);

	//Register as a Window listener
	WindowEventUtilities::addWindowEventListener(mWindow, this);

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);
	
	//GUISystem::GetInstance()->Init(mWindow);
}

AAFrameListener::~AAFrameListener()
{
	if(mInputManager)
	{
		mInputManager->destroyInputObject(mMouse);
		mInputManager->destroyInputObject(mKeyboard);
		OIS::InputManager::destroyInputSystem(mInputManager);
		mInputManager = 0;			
	}
	//Remove ourself as a Window listener
	WindowEventUtilities::removeWindowEventListener(mWindow, this);
	windowClosed(mWindow);
}

float AAFrameListener::GetFPS()
{
    return mWindow->getLastFPS();    
}

void AAFrameListener::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";

	// update stats when necessary
	OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
	OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
	OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
	OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

	guiAvg->setCaption(avgFps + StringConverter::toString(mWindow->getAverageFPS()));
	guiCurr->setCaption(currFps + StringConverter::toString(mWindow->getLastFPS()));
	guiBest->setCaption(bestFps + StringConverter::toString(mWindow->getBestFPS())
		+" "+StringConverter::toString(mWindow->getBestFrameTime())+" ms");
	guiWorst->setCaption(worstFps + StringConverter::toString(mWindow->getWorstFPS())
		+" "+StringConverter::toString(mWindow->getWorstFrameTime())+" ms");

	OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
	guiTris->setCaption(tris + StringConverter::toString(mWindow->getTriangleCount()));

	OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	Ogre::Vector3 v = Core::GetInstance()->GetPlayer()->GetPosition();
	char dbg[100];
	sprintf(dbg,"x: %.0f y %.0f z %.0f",v.x,v.y,v.z);
	mDebugText = dbg;
	guiDbg->setCaption(mDebugText);
}

//Adjust mouse clipping area
void AAFrameListener::windowResized(RenderWindow* rw)
{
	unsigned int width, height, depth;
	int left, top;
	rw->getMetrics(width, height, depth, left, top);

	const OIS::MouseState &ms = mMouse->getMouseState();
	ms.width = width;
	ms.height = height;
}

//Unattach OIS before window shutdown (very important under Linux)
void AAFrameListener::windowClosed(RenderWindow* rw)
{	
	//Only close for window that created OIS (the main window in these demos)
	if( rw == mWindow )
	{
		if( mInputManager )
		{
			mInputManager->destroyInputObject( mMouse );
			mInputManager->destroyInputObject( mKeyboard );
			//mInputManager->destroyInputObject( mJoy );

			OIS::InputManager::destroyInputSystem(mInputManager);
			mInputManager = 0;
		}
	}
}

bool AAFrameListener::processUnbufferedKeyInput(const FrameEvent& evt)
{
	using namespace OIS;		

	//if( mKeyboard->isKeyDown(KC_ESCAPE) )
	//{
	//	//CommonDeclarations::GetApplication()->SwitchListener();
	//	CommonDeclarations::GetApplication()->SetCurrentGameState(AApplication::GS_GUI);
	//	GUISystem::GetInstance()->GetMainMenu()->Show();
	//	return true;
	//	//mShutdownRequested = true;
	//	//return false;
	//}
	
	if (Blocked)
		return true;

	if( mKeyboard->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0 ) 
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);
		mTimeUntilNextToggle = 1;
	}

	if(mKeyboard->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		std::ostringstream ss;
		ss << "screenshot_" << ++mNumScreenShots << ".png";
		mWindow->writeContentsToFile(ss.str());
		mTimeUntilNextToggle = 0.5;
		mDebugText = "Saved: " + ss.str();
	}
	
	CoreInstance->processKeyInput(mKeyboard);

	// Return true to continue rendering
	return true;
}

bool AAFrameListener::processUnbufferedMouseInput(const FrameEvent& evt)
{
	using namespace OIS;
	
	if (Blocked)
		return true;

    const MouseState &ms = mMouse->getMouseState();
    PlayersCamera->ProcessMouseInput(ms);

	CoreInstance->processMouseInput(mMouse);

	return true;
}

//void AAFrameListener::moveCamera()
//{		
//	if (Blocked)
//		return;
//    PlayersCamera->Update();
//}

void AAFrameListener::showDebugOverlay(bool show)
{   
	Overlay* o = OverlayManager::getSingleton().getByName("Core/DebugOverlay");
	if (!o)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay Core/DebugOverlay",
		"showDebugOverlay" );
	if (show)
		o->show();
	else
		o->hide();
}

#include "Debugging.h"

bool AAFrameListener::frameStarted(const FrameEvent& evt)
{   
	//static Ogre::Real ActualTimeSinceLastFrame = 0;
	//ActualTimeSinceLastFrame += evt.timeSinceLastFrame;
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
    
	if (mTimeUntilNextToggle >= 0)
		mTimeUntilNextToggle -= evt.timeSinceLastFrame;
	
	if( processUnbufferedKeyInput(evt) == false )
		return false;

	if( processUnbufferedMouseInput(evt) == false )
		return false;
   	
	CoreInstance->Step(ticks);	

	return true;
}

// Override frameEnded event 
bool AAFrameListener::frameEnded(const FrameEvent& evt)
{	
	if (mShutdownRequested)
		return false;

	updateStats();
	return true;
}

/// Tell the frame listener to exit at the end of the next frame
void AAFrameListener::requestShutdown(void)
{
	mShutdownRequested = true;
}

//----------------------------------------------------------------//
bool AAFrameListener::mouseMoved( const OIS::MouseEvent &arg )
{
	
	return true;
}

//----------------------------------------------------------------//
bool AAFrameListener::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	
	return true;
}

//----------------------------------------------------------------//
bool AAFrameListener::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	
	return true;
}

//----------------------------------------------------------------//
bool AAFrameListener::keyPressed( const OIS::KeyEvent &arg )
{
	if( arg.key == OIS::KC_ESCAPE )
		mShutdownRequested = true;
	return true;
}

//----------------------------------------------------------------//
bool AAFrameListener::keyReleased( const OIS::KeyEvent &arg )
{

	return true;
}

void AAFrameListener::SetBlocked(bool on)
{
	Blocked=on;
}



/*
-----------------------------------------------------------------------------
Filename:    FrameListener.h
Description: Defines an example frame listener which responds to frame events.
             This frame listener just moves a specified camera around based on
             keyboard and mouse movements.
             Mouse:    Freelook
             W or Up:  Forward
             S or Down:Backward
             A:           Step left
             D:        Step right
             PgUp:     Move upwards
             PgDown:   Move downwards
             O/P:       Yaw the root scene node (and it's children)
             I/K:       Pitch the root scene node (and it's children)
             F:           Toggle frame rate stats on/off
-----------------------------------------------------------------------------
*/

#pragma once
#include "OgreFrameListener.h"
#include "OgreRenderWindow.h"
#include "OgreVector3.h"
#include "OgreOverlayManager.h"
#include "OgreOverlayElement.h"
#include "OgreStringConverter.h"
#include "OgreCamera.h"
#include "OgreWindowEventUtilities.h"

#include "OgreException.h"
#define OIS_DYNAMIC_LIB
#include <OIS.h>

#include "InterpolatedRotation.h"

using namespace Ogre;

class Core;
class Player;
class AACamera;

class AAFrameListener: public FrameListener, public WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
private:
    void updateStats(void);
    
public:
    // Constructor takes a RenderWindow because it uses that to determine input context
	AAFrameListener(Ogre::RenderWindow* win, AACamera* cam, bool bufferedKeys = false, bool bufferedMouse = false);

	//Adjust mouse clipping area
	virtual void windowResized(RenderWindow* rw);

	//Unattach OIS before window shutdown (very important under Linux)
	virtual void windowClosed(RenderWindow* rw);

    virtual ~AAFrameListener();

    bool processUnbufferedKeyInput(const FrameEvent& evt);

    bool processUnbufferedMouseInput(const FrameEvent& evt);

	//void moveCamera();

    void showDebugOverlay(bool show);

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const FrameEvent& evt);

    // Override frameEnded event 
    bool frameEnded(const FrameEvent& evt);

	/// Tell the frame listener to exit at the end of the next frame
	void requestShutdown(void);

	//----------------------------------------------------------------//
	bool mouseMoved( const OIS::MouseEvent &arg );

	//----------------------------------------------------------------//
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	//----------------------------------------------------------------//
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	//----------------------------------------------------------------//
	//----------------------------------------------------------------//
	bool keyPressed( const OIS::KeyEvent &arg );

	//----------------------------------------------------------------//
	bool keyReleased( const OIS::KeyEvent &arg );

    float GetFPS();
	
	void SetBlocked(bool on);

protected:
	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
    OIS::InputManager *mInputManager;
	    
    AACamera* PlayersCamera;
    RenderWindow* mWindow;
    bool mStatsOn;
	unsigned int mNumScreenShots;
    // just to stop toggles flipping too fast
    Real mTimeUntilNextToggle ;

	std::string mDebugText;
	bool mShutdownRequested;

	// just to stop toggles flipping too fast
	TextureFilterOptions mFiltering;
	int mAniso;

	int mSceneDetailIndex ;
	Overlay* mDebugOverlay;

	Core *CoreInstance;
    Real Time;
	
	bool Blocked;    
};


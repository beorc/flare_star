#pragma once
#include <MyGUI.h>
#include <Ogre.h>

#include <OgreFrameListener.h>
#include "OgreRenderWindow.h"
#include "OgreWindowEventUtilities.h"
#include "OgreException.h"
#define OIS_DYNAMIC_LIB
#include <OIS.h>

class IFrameHandler;

class CombatFrameListener : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
public:    
    ~CombatFrameListener(void);

    static CombatFrameListener *GetInstance();    

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const Ogre::FrameEvent& evt);

	// Override frameEnded event 
	bool frameEnded(const Ogre::FrameEvent& evt);

	/// Tell the frame listener to exit at the end of the next frame
	void requestShutdown(void);

	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool keyPressed( const OIS::KeyEvent &arg );	
	bool keyReleased( const OIS::KeyEvent &arg );

	void windowResized(Ogre::RenderWindow* rw);

	void RequestShutDown();

	bool Init(Ogre::RenderWindow* window);
	void Close();

    void SetCurrentHandler(IFrameHandler *);
	IFrameHandler *GetCurrentHandler() const;

	void SetLastFrameTime(clock_t);

	
protected:	
    CombatFrameListener(void);

	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;	
	Ogre::RenderWindow* mWindow;    
	
	bool mShutdownRequested;
	IFrameHandler *CurrentHandler;
	clock_t LastFrameTime;
};

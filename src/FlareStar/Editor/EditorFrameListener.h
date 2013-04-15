#pragma once
#include <MyGUI.h>
#include <Ogre.h>

#include <OgreFrameListener.h>
#include "OgreRenderWindow.h"
#include "OgreWindowEventUtilities.h"
#include "OgreException.h"
#define OIS_DYNAMIC_LIB
#include <OIS.h>

class EditorFrameHandler;

class EditorFrameListener : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
public:    
    ~EditorFrameListener(void);

    static EditorFrameListener *GetInstance();    

	// Override frameStarted event to process that (don't care about frameEnded)
	bool frameStarted(const Ogre::FrameEvent& evt);

	// Override frameEnded event 
	bool frameEnded(const Ogre::FrameEvent& evt);

	bool mouseMoved( const OIS::MouseEvent &arg );
	bool mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	bool keyPressed( const OIS::KeyEvent &arg );	
	bool keyReleased( const OIS::KeyEvent &arg );

	void windowResized(Ogre::RenderWindow* rw);

	void RequestShutDown();

	bool Init(Ogre::RenderWindow* window);
	void Close();

	
	/*void Show();
	void Hide();*/

	//MyGUI::Gui *GetGui();
	OIS::Mouse *GetMouse() const;

    void SetCurrentHandler(EditorFrameHandler *);
	EditorFrameHandler *GetCurrentHandler() const;

	void SetLastFrameTime(clock_t);

	//EditorFrameHandler *GetEditorHandler() const;
	
protected:	
    EditorFrameListener(void);

	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;	
	Ogre::RenderWindow* mWindow;    
	
	bool mShutdownRequested;
	EditorFrameHandler *CurrentHandler;
	clock_t LastFrameTime;

	//MyGUI::Gui *GUI;
};

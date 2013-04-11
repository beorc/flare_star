#pragma once
#include <MyGUI.h>
#include <Ogre.h>

#include <OgreFrameListener.h>
#include "OgreRenderWindow.h"
#include "OgreWindowEventUtilities.h"
#include "OgreException.h"
#define OIS_DYNAMIC_LIB
#include <OIS.h>

class GUILayout;
class GUIIntroMenuLayout;
class GUIMainMenuLayout;
class GUIRadarLayout;
class GUIOptionsLayout;
class GUINewGameLayout;
class GUILoadGameLayout;
class IDescriptable;

class GUISystem : public Ogre::FrameListener, public Ogre::WindowEventListener, public OIS::KeyListener, public OIS::MouseListener
{
public:
    ~GUISystem(void);

    static GUISystem *GetInstance();    

    bool Init(Ogre::RenderWindow* window);
    static void Close();
	void Reset();

	void Show();
	void Hide();

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

	MyGUI::Gui *GetGui();

	GUILayout *GetIntroMenu();
	GUILayout *GetMainMenu();
	GUILayout *GetOptions();
    GUILayout *GetRadar();
	GUILayout *GetNewGameMenu();

	void NewGameHandler(GUILayout *back_layout=NULL);
	void LoadGameHandler(GUILayout *back_layout=NULL);
	
	void SetHudVisible(bool on);
	void ShowDescription(IDescriptable *descr);
	void HideDescription();

	void FlushInput();
	
protected:	
    GUISystem(void);
    //static GUISystem *Instance;
    
    std::auto_ptr<MyGUI::Gui> GUI;
	Ogre::Rectangle2D *IntroMenuBackground;

	OIS::Mouse *mMouse;
	OIS::Keyboard *mKeyboard;
	OIS::InputManager *mInputManager;	
	Ogre::RenderWindow* mWindow;    
	
	bool mShutdownRequested;

	std::auto_ptr<GUIIntroMenuLayout> IntroMenu;
	std::auto_ptr<GUIMainMenuLayout> MainMenu;
	std::auto_ptr<GUIRadarLayout> Radar;
	std::auto_ptr<GUIOptionsLayout> Options;
	std::auto_ptr<GUINewGameLayout> NewGame;
	std::auto_ptr<GUILoadGameLayout> LoadGame;
};

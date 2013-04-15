#pragma once

#include "OgreRoot.h"

#include "OgreConfigFile.h"
#include "AAFrameListener.h"
#include "OgreMax/OgreMaxScene.hpp"
//#include <string>

class AACamera;

using namespace Ogre;

class Player;

class OgreMax::OgreMaxScene;

class AACommandsFrameListener;

//class StudioIntroHandler;

/** Base class which manages the standard startup of an Ogre application
    based on the ReferenceApplication layer.
    Designed to be subclassed for specific examples if required.
*/
class AApplication
{
public:
	enum EGameStates {GS_NONE, GS_GAME, GS_GUI, GS_COMIX, GS_STUDIOINTRO, GS_ENGINESCENE, GS_COMBAT, GS_EDITOR};

    /// Standard constructor
	AApplication();
    /// Standard destructor
    virtual ~AApplication();

    /// Start the example
    virtual void go(void);
	virtual void StartGame(void);
	virtual void ResetGame(void);

	void LoadScene(const char *filename);
	//Ogre::SceneManager *GetSceneManager() {return mSceneMgr;}

    void ParseCommandLine(LPTSTR    lpCmdLine);

	virtual void createCamera();
	void LinkCamera(Player *players_object);
	
    //void SwitchListener();

	void InitGui();
	
	Ogre::Camera *GetScriptsCamera();

	void SetCurrentCamera(Ogre::Camera *cam);
	void SetCurrentGameState(int state);
	int GetCurrentGameState() const;

	Ogre::Viewport* GetCurrentViewPort() const;
	Ogre::RenderWindow* GetCurrentRenderWindow() const;

	Ogre::Camera *GetCombatCamera();
protected:
    Root *mRoot;
    
	AACamera *PlayersCamera;
    SceneManager* mSceneMgr;
    AAFrameListener* mFrameListener;	
    Ogre::FrameListener *CurrentListener;
    RenderWindow* mWindow;
	//SceneNode* mSceneNode;
	//Ogre::SceneNode *CameraTransformationNode, *PlayerTransformationNode;	
	Player *PlayersObject;
	OgreMax::OgreMaxScene SceneLoader;
	Ogre::Camera *ScriptsCamera;
	//std::string HomePath;

    // These internal methods package up the stages in the startup process
    /** Sets up the application - returns false if the user chooses to abandon configuration. */
    virtual bool setup(void);
    /** Configures the application - returns false if the user chooses to abandon configuration. */
    virtual bool configure(void);

    virtual void chooseSceneManager(void);
    
    virtual void createFrameListener(void);

    virtual void createScene(void);

    virtual void createViewports(void);

    /// Method which will define the source of resources (other than current folder)
    virtual void setupResources(std::string path);
	void RemoveResourceLocations(std::string path);
	/// Optional override method where you can create resource listeners (e.g. for loading screens)
	virtual void createResourceListener(void);

	/// Optional override method where you can perform resource group loading
	/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	virtual void loadResources(void);

    bool NeedConfigure;
	clock_t ListenerSwitchTime;
	Ogre::Viewport* Vp;

	EGameStates CurrentGameState;
    
	Ogre::Camera *CombatCamera;
};


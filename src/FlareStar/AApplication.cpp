#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AApplication.h"
#include "Core.h"
//#include "CommonDeclarations.h"
#include "Player.h"
#include "EntityMaterialInstance.h"
#include "AACamera.h"
#include "GUISystem.h"
#include "GUILayout.h"
#include "Profile.h"
#include "ScriptManager.h"
#include "ComixFrameListener.h"
#include "ComixFrameHandler.h"
#include "AACommandsFrameListener.h"
#include "ObjectsLibrary.h"
#include "Debugging.h"
#include "HUDCentre.h"
#include "Editor\EditorFrameListener.h"
#include "Combat\CombatFrameListener.h"

AApplication::AApplication() :
mFrameListener(NULL),
mRoot(NULL),
PlayersCamera(NULL),
NeedConfigure(false),
CurrentListener(NULL),
ListenerSwitchTime(0),
Vp(0),
CurrentGameState(GS_NONE),
CombatCamera(NULL)
{
}

/// Standard destructor
AApplication::~AApplication()
{
	SceneLoader.Destroy();
	Core::GetInstance()->Close();	
	if (mFrameListener)
		delete mFrameListener;
	if (mRoot)
		delete mRoot;	
}

void AApplication::createScene(void)
{
	Core *Instance = Core::GetInstance();	
	//Instance->SetSceneManager(mSceneMgr);
	//Instance->SetApplication(this);
	
	//if (!Instance->LoadLevel(Instance->GetLevelPath()))
	if (!Instance->LoadLevel("level.xml"))
		return;		

}

void AApplication::createFrameListener(void)
{
	mFrameListener= new AAFrameListener(mWindow, PlayersCamera,false,false);	
	
    Core::GetInstance()->SetFrameListener(mFrameListener);
    //CurrentListener = mFrameListener;	
	//CurrentListener = GUISystem::GetInstance();
	//mRoot->addFrameListener(CurrentListener);
}

void AApplication::go(void)
{
	if (!setup())
		return;

	mRoot->startRendering();	
}

void AApplication::ResetGame(void)
{	
	
	PlayersCamera->UnlinkFromPlayer();		
	PlayersCamera->Reset();	
	Core::GetInstance()->Reset();			
	
	SceneLoader.Destroy();		
	mSceneMgr->clearScene();

	Ogre::ParticleSystemManager::getSingleton().removeAllTemplates(true);	

	AAProfile *profile = Core::GetInstance()->GetProfile();
	if (profile->CurrentLevel>0 && strcmp(profile->ProfileName, "default.xml")!=0)
	{
		/*Ogre::StringVector groups =  ResourceGroupManager::getSingleton().getResourceGroups();
		Ogre::StringVector::iterator iPos = groups.begin(), iEnd = groups.end();
		for (;iPos!=iEnd;++iPos)
		{
			if (*iPos == LEVEL_RESOURCE_GROUP)		
			{*/
				ResourceGroupManager::getSingleton().clearResourceGroup(LEVEL_RESOURCE_GROUP);
			/*}
		}	*/
		RemoveResourceLocations(Core::GetInstance()->GetLevelPath());		
		Core::GetInstance()->GetProfile()->Reset();
	}	
}

void AApplication::StartGame(void)
{	
	Core::GetInstance()->LoadProfile();
	setupResources(Core::GetInstance()->GetLevelPath());

	ResourceGroupManager::getSingleton().initialiseResourceGroup(LEVEL_RESOURCE_GROUP);

	ResourceGroupManager::getSingleton().loadResourceGroup(LEVEL_RESOURCE_GROUP, true, true);

	
	//loadResources();

	//MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Sky");
	//mat->setFog(true, Ogre::FOG_LINEAR, Ogre::ColourValue::White, 0.001, 0, 100);

	//GUISystem::GetInstance()->GetRadar()->Load();

    //MeshPtr m = MeshManager::getSingleton().load("player.mesh", LEVEL_RESOURCE_GROUP);
    //unsigned short src=0, dest=0;
    ////if (!m->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
    //{
    //    m->suggestTangentVectorBuildParams(VES_TANGENT, src, dest);
    //    m->buildTangentVectors(VES_TANGENT, src, dest);
    //}

	Core::GetInstance()->CreatePlayer();

	ScriptManager::GetInstance()->Init();

	createScene();
	//mWindow->getViewport(0)->setBackgroundColour(ColourValue(1,1,1));
	//createCamera();
	//mSceneMgr->setFog(Ogre::FOG_LINEAR, Ogre::ColourValue::White, 0.001f, 5000.f, 10000.f);
	LinkCamera(Core::GetInstance()->GetPlayer());
	SetCurrentCamera(PlayersCamera->GetOgreCamera());
	//createViewports();
	//InitGui();

	//createFrameListener();

	//SwitchListener();
	SetCurrentGameState(AApplication::GS_GAME);

	CommonDeclarations::PlayIntroMusic(false);
    
	Core::GetInstance()->Start();
}

// These internal methods package up the stages in the startup process
/** Sets up the application - returns false if the user chooses to abandon configuration. */
bool AApplication::setup(void)
{
	mRoot = new Root();
	
	setupResources("media/common/");
	    
	bool carryOn = configure();
	if (!carryOn) return false;

	chooseSceneManager();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);
		
	ResourceGroupManager::getSingleton().initialiseResourceGroup("Bootstrap");
	ResourceGroupManager::getSingleton().initialiseResourceGroup(GUI_RESOURCE_GROUP);
	ResourceGroupManager::getSingleton().initialiseResourceGroup("General");

	ScriptsCamera = mSceneMgr->createCamera("ScriptsCamera");
	ScriptsCamera->setNearClipDistance(5);		
	ScriptsCamera->setFarClipDistance(0);

	CombatCamera = mSceneMgr->createCamera("CombatCamera");
	CombatCamera->setNearClipDistance(5);		
	CombatCamera->setFarClipDistance(0);

	Ogre::SceneNode *node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	node->attachObject(ScriptsCamera);	
	
	createCamera();
	createViewports();
	
	Core *Instance = Core::GetInstance();	
	Instance->SetSceneManager(mSceneMgr);
	Instance->SetApplication(this);

	InitGui();

	if (0!=Instance->Init())
		return false;

	//CurrentListener = GUISystem::GetInstance();
	//mRoot->addFrameListener(CurrentListener);
    
    /*MaterialPtr fade = Ogre::MaterialManager::getSingleton().create("FadeMaterial", "General");    
    fade->getTechnique(0)->getPass(0)->setSceneBlending(SBT_TRANSPARENT_ALPHA);*/

	createFrameListener();

	AACommandsFrameListener *commands_listener = AACommandsFrameListener::GetInstance();
	commands_listener->Init(mWindow);	
	commands_listener->SetCurrentHandler(Instance);
	mRoot->addFrameListener(commands_listener);

	EditorFrameListener *editor_listener = EditorFrameListener::GetInstance();
	editor_listener->Init(mWindow);	
	
	CombatFrameListener::GetInstance()->Init(mWindow);

    ComixFrameListener::GetInstance()->Init(mWindow);
    //SetCurrentGameState(AApplication::GS_STUDIOINTRO);
    SetCurrentGameState(AApplication::GS_COMIX);

    ComixFrameHandler *handler = new ComixFrameHandler();                        
    TiXmlElement *xml = ObjectsLibrary::GetParsed("logo_intro.xml");
    handler->Parse(xml);
    delete xml;
    ComixFrameListener *listener = ComixFrameListener::GetInstance();
    listener->SetCurrentHandler(handler);

	CommonDeclarations::PlayIntroMusic(true);
	
    //handler->Show(0);	
    //handler->SetActive(0, true);

	//handler->Show(1);
	//handler->SetActive(1, true);
    
    //SetCurrentGameState(AApplication::GS_GUI);	
	//GUISystem::GetInstance()->GetIntroMenu()->Show();
	
    //GUISystem::GetInstance()->Show();	
		
    return true;
}
/** Configures the application - returns false if the user chooses to abandon configuration. */
bool AApplication::configure(void)
{
	// Show the configuration dialog and initialise the system
	// You can skip this and use root.restoreConfig() to load configuration
	// settings if you were sure there are valid ones saved in ogre.cfg
    if (mRoot->restoreConfig() && !NeedConfigure)
    {
        mWindow = mRoot->initialise(true, "Flare Star Demo");
        return true;
    } else
	if(mRoot->showConfigDialog())
	{
		// If returned true, user clicked OK so initialise
		// Here we choose to let the system create a default rendering window by passing 'true'
		mWindow = mRoot->initialise(true, "Flare Star Demo");
		return true;
	}
	else
	{
		return false;
	}
}

void AApplication::chooseSceneManager(void)
{
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->createSceneManager(ST_GENERIC, "RefAppSMInstance");
}	

void AApplication::createCamera()
{
	PlayersCamera = new AACamera(mSceneMgr->createCamera("PlayersCamera"));
	//MALLOC(PlayersCamera, AACamera, mSceneMgr->createCamera("PlayersCamera")); // ScriptsCamera
	Core::GetInstance()->SetCamera(PlayersCamera);	
	
	PlayersCamera->GetOgreCamera()->setNearClipDistance(5);
	PlayersCamera->GetOgreCamera()->setFarClipDistance(0);
}

void AApplication::LinkCamera(Player *players_object)
{
	PlayersObject = players_object;
	Ogre::SceneNode *CameraTransformationNode;
	CameraTransformationNode = PlayersObject->GetNode()->createChildSceneNode("CameraTransformation");
	CameraTransformationNode->setInheritOrientation(false);

    //
	/*Ogre::Light *mLight = mSceneMgr->createLight("Light2");
	mLight->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
	mLight->setSpecularColour(1, 1, 1);
	mLight->setAttenuation(80000,1,1,0);*/

	// Create light node
	//Ogre::SceneNode *mLightNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
	//	"MovingLightNode");
 //   mLightNode->setInheritOrientation(false);
	//mLightNode->attachObject(mLight);

	//mLightNode->setPosition(0, 0, -96000);

 //   BillboardSet* bbs = mSceneMgr->createBillboardSet("lightbbs", 1);
 //   bbs->setMaterialName("Bullets/Bullet1");
 //   Billboard* bb = bbs->createBillboard(0,0,0,Ogre::ColourValue(0.5, 0.1, 0.0));
 //   // attach
 //   mLightNode->attachObject(bbs);

    // Put an Ogre head in the middle
    //MeshPtr m = MeshManager::getSingleton().load("ogrehead.mesh", 
    //    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    //unsigned short src, dest;
    //if (!m->suggestTangentVectorBuildParams(VES_TANGENT, src, dest))
    //{
    //    m->buildTangentVectors(VES_TANGENT, src, dest);
    //}
    //Entity* e;
    //e = mSceneMgr->createEntity("head", "ogrehead.mesh");
    //e->setMaterialName("Examples/OffsetMapping/Specular");
    ////e->setMaterialName("FxNormalMap");

    //SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
    //headNode->attachObject(e);
    //headNode->setScale(7,7,7);
    //headNode->setPosition(0,0,-95000);
    //e->setNormaliseNormals(true);    
    //
	
	PlayersCamera->SetNode(CameraTransformationNode);
	//Core::GetInstance()->SetCamera(PlayersCamera);	

    CameraTransformationNode->setPosition(Ogre::Vector3::ZERO);
	PlayersCamera->GetOgreCamera()->setOrientation(Ogre::Quaternion::IDENTITY);

	PlayersCamera->SetInitialOrientation();
	PlayersCamera->SetPlayersObject(PlayersObject);	

	Core::GetInstance()->AddObjectRequest(PlayersCamera);
}

void AApplication::createViewports(void)
{
	// Create one viewport, entire window
	Vp = mWindow->addViewport(PlayersCamera->GetOgreCamera());	
}

/// Method which will define the source of resources (other than current folder)
void AApplication::setupResources(std::string path)
{
	// Load resource paths from config file

	ConfigFile cf;
	cf.load(path+"resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}

}

void AApplication::RemoveResourceLocations(std::string path)
{
	// Load resource paths from config file

	ConfigFile cf;
	cf.load(path+"resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().removeResourceLocation(archName, secName);
		}
	}

}

/// Optional override method where you can create resource listeners (e.g. for loading screens)
void AApplication::createResourceListener(void)
{

}

/// Optional override method where you can perform resource group loading
/// Must at least do ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
void AApplication::loadResources(void)
{
	// Initialise, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

}

void AApplication::LoadScene(const char *filename)
{
	SceneLoader.Load(filename, mWindow, OgreMax::OgreMaxModel::NO_OPTIONS, mSceneMgr, mSceneMgr->getRootSceneNode(), Core::GetInstance(),Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);	
}

void AApplication::ParseCommandLine(LPTSTR    lpCmdLine)
{
    if (_tcscmp(lpCmdLine,_T("/cfg"))==0)
        NeedConfigure = true;
}

//void AApplication::SwitchListener()
//{
//	if (clock()-ListenerSwitchTime<500 || !mFrameListener || !GUISystem::GetInstance())
//		return;
//
//	ListenerSwitchTime = clock();
//
//    mRoot->removeFrameListener(CurrentListener);
//    if (CurrentListener==mFrameListener)
//    {        
//        CurrentListener = GUISystem::GetInstance();
//		GUISystem::GetInstance()->Show();
//    }
//    else
//	{     
//		CurrentListener = mFrameListener;
//		GUISystem::GetInstance()->Hide();
//	}
//    mRoot->addFrameListener(CurrentListener);
//}

void AApplication::InitGui()
{
	GUISystem::GetInstance()->Init(mWindow);
}

Ogre::Camera *AApplication::GetScriptsCamera()
{
	return ScriptsCamera;
}

void AApplication::SetCurrentCamera(Ogre::Camera *cam)
{
	Vp->setCamera(cam);
}

int AApplication::GetCurrentGameState() const
{
	return CurrentGameState;
}

void AApplication::SetCurrentGameState(int state)
{
	EGameStates game_state = (EGameStates)state;

	if (CurrentGameState == game_state)
		return;

	mRoot->removeFrameListener(CurrentListener);
	
	CurrentGameState = game_state;

	
	AACommandsFrameListener *commands_listener = AACommandsFrameListener::GetInstance();
		
	/*char log[100];
	sprintf(log,"%d\n",CurrentGameState);
	Debugging::Log("gstate",log);*/
    
	switch(game_state) {
	case GS_ENGINESCENE:
		{
			HUDCentre::GetInstance()->Hide();
            CommonDeclarations::SetSensorVisible(false);
			//mRoot->removeFrameListener(commands_listener);
			CurrentListener = mFrameListener;
			GUISystem::GetInstance()->Hide();
			mRoot->addFrameListener(commands_listener);
			break;
		}
	case GS_GAME:
		{
			HUDCentre::GetInstance()->Show();
            CommonDeclarations::SetSensorVisible(true);
			CurrentListener = mFrameListener;
			GUISystem::GetInstance()->Hide();
			commands_listener->ResetBuffer();
			mRoot->addFrameListener(commands_listener);
			break;	
		}
	case GS_GUI:
		{
			HUDCentre::GetInstance()->Hide();
            CommonDeclarations::SetSensorVisible(false);
			mRoot->removeFrameListener(commands_listener);
            //if (StudioHandler)
            //{
            //    //listener->SetCurrentHandler(NULL);
            //    delete StudioHandler;
            //    StudioHandler = NULL;
            //}
			CurrentListener = GUISystem::GetInstance();
			GUISystem::GetInstance()->Show();
			break;
		}
	case GS_COMIX:
		{
			HUDCentre::GetInstance()->Hide();
            CommonDeclarations::SetSensorVisible(false);
			mRoot->removeFrameListener(commands_listener);

            ComixFrameListener *listener = ComixFrameListener::GetInstance();
            CurrentListener = listener;

			GUISystem::GetInstance()->Hide();
			break;	
		}
	case GS_COMBAT:
		{
			HUDCentre::GetInstance()->Hide();
            CommonDeclarations::SetSensorVisible(false);
			mRoot->removeFrameListener(commands_listener);

            CombatFrameListener *listener = CombatFrameListener::GetInstance();
            CurrentListener = listener;

			GUISystem::GetInstance()->Hide();
			break;	
		}
	case GS_EDITOR:
		{
			HUDCentre::GetInstance()->Hide();
            CommonDeclarations::SetSensorVisible(false);
			mRoot->removeFrameListener(commands_listener);
            
			CurrentListener = EditorFrameListener::GetInstance();
			//EditorFrameListener::GetInstance()->Show();

			break;
		}
	};

	mRoot->addFrameListener(CurrentListener);
}

Ogre::Viewport* AApplication::GetCurrentViewPort() const
{
	return Vp;
}

Ogre::RenderWindow* AApplication::GetCurrentRenderWindow() const
{
	return mWindow;
}

Ogre::Camera *AApplication::GetCombatCamera()
{
	return CombatCamera;
}

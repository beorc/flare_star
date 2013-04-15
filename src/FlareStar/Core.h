#pragma once
#include "XMLConfigurable.h"
#include "OgreMax/OgreMaxScene.hpp"
#include <OgreAL.h>

//#ifdef MEMORY_POOL
//#include "MemoryPool.h"
//#endif

#include <vector>
#include "List.h"
#include "Stall.h"
#include "CommonDeclarations.h"
#include "IFrameHandler.h"

class Player;
class Incubator;
class Weapon;
class Ammo;

class AApplication;
class AAFrameListener;
class IAAObject;
class ScriptHandlersManager;
class RoomsManager;
class AACamera;
class IAAObject;
//class Configuration;
class CollisionModel3D;
class Room;
struct AAProfile;
class ActivationTrigger;

#define OIS_DYNAMIC_LIB
#include <OIS.h>
//#include <hash_map>

class Core : public XMLConfigurable, public OgreMax::OgreMaxSceneCallback, public IFrameHandler
{
public:	    
	enum {IDObjectsCacheSize=200};	
	typedef List<IAAObject*> TempObjectsPool;
		
    typedef List<ActivationTrigger*> TriggersContainer;
	
	~Core(void);

	static Core *GetInstance();
	
	void SetCamera(AACamera *camera);
	AACamera *GetCamera();
	Player *GetPlayer();

	CommonDeclarations::ObjectsPool *GetObjects();
	
	//int LoadScene(OgreMax::OgreMaxScene *loader);
	void Step(unsigned timeMs);
	
	virtual bool ParseOne(TiXmlElement* xml);
	virtual bool ParseScenes(TiXmlElement* xml);    
	
	void SetApplication(AApplication *app) {OgreApplication=app;}
	AApplication *GetApplication() {return OgreApplication;}

	void SetSceneManager(Ogre::SceneManager *sm) {SceneManager=sm;}
	Ogre::SceneManager *GetSceneManager() {return SceneManager;}

	bool processMouseInput(const OIS::Mouse *mouse);
	bool processKeyInput(OIS::Keyboard *keyboard);

	static char* GenGUID();

	void DeleteObjectRequest(IAAObject *object);
	void AddObjectRequest(IAAObject *object);

	void RegisterObject(IAAObject *object);

    float GetFPS();
    void SetFrameListener(AAFrameListener *listener);

	bool LoadLevel(const char* filename);

	void SetMusicGain(float gain);
	float GetMusicGain() const;

	// callbacks

	virtual void CreatedEntity(const OgreMax::OgreMaxScene* scene, Ogre::Entity* entity);
	virtual void StartedCreatingNode(const OgreMax::OgreMaxScene* scene, Ogre::SceneNode* node);	
	virtual void FinishedCreatingNode(const OgreMax::OgreMaxScene* scene, Ogre::SceneNode* node);
	virtual void HandleObjectExtraData(OgreMax::Types::ObjectExtraDataPtr objectExtraData);

	RoomsManager *GetRoomManager();

    void RemoveIDObject(int id);
    void AddIDObject(IAAObject *obj);
    IAAObject *GetIDObject(int id);
    int GetNewID();

	bool ExcludeObjectFromQueue(IAAObject *obj);
	bool IncludeObjectToQueue(IAAObject *obj);
	void ActivateObject(int id, bool on);
    
    OgreAL::SoundManager *GetSoundManager();

	CommonDeclarations::CollisionObjectsPool *GetCollisionObjectsPool();
    //const CommonDeclarations::RadarObjectsArray *GetRadarObjects() const;
	
	int Init();
	void Start();	
	void Reset();
	void Close();

	bool SaveProfile();
	bool LoadProfile();

	bool LoadGameConfig();

	const char* GetLevelPath();
	const char* GetCurrentProfileName();
	void SetCurrentProfileName(const char* profile_name);

	AAProfile *GetProfile() const;

	void PlayerKilled();
	void CreatePlayer();

    void AddCreatedObjects();
	void BlockPlayer(bool on);

	//void AddToDestroyFirst(IAAObject *obj);

	//void SetScriptHandlersActive(bool on);
    Ogre::Vector3 GetUpVector() const;

	void SetScriptHandler(int handler_num, char *handler_name, int timeout);

	virtual bool KeyPressed( const OIS::KeyEvent &arg );
	virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

	virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
	virtual bool MouseMoved( const OIS::MouseEvent &arg );
	virtual bool KeyReleased( const OIS::KeyEvent &arg );

    void SetAutoPilotActive( bool on );
    
    void SetSensorVisible( bool on );

	void PlayIntroMusic(bool on);

    TiXmlElement* GetGameConfig() const;
	ActivationTrigger *GetTriggerByID(int id);

	CommonDeclarations::ObjectsPool *GetEditableObjects();

	
	//CommonDeclarations::IDObjectsPool *GetIDObjects();
	

private:
	enum EScriptHandlerType {ESHT_TIMER};
	Core(void);

	//static Core *Instance;

	//PhysicsEngine *PhysicsEngineInstance;
	
	Player *PlayersObject;
	
	AACamera* Camera;
	Ogre::SceneManager *SceneManager;
    //Configuration *ConfigurationInstance;
	
	AApplication *OgreApplication;
	AAFrameListener *OgreFrameListener;

	Ogre::SceneNode *CreatingNode;
	//Ogre::Entity *CreatingEntity;
	IScenable *CreatingObject;
	bool			DropCreatedNode, DropCreatedEntity;

	TempObjectsPool ObjectsToAdd, ObjectsToDel;
	CommonDeclarations::ObjectsPool Objects;
    TriggersContainer ActivationTriggers;
	//std::auto_ptr<ScriptHandlersManager> ScriptHandlersModule;

	// levels & scenes
	RoomsManager *LevelRoomManager;

    OgreAL::SoundManager *SoundManager;
    OgreAL::Sound *BackGroundSound, *IntroSound;

    //void UpdateMusic(unsigned timeMs);
    void MusicFinished(OgreAL::Sound *sound);

    CommonDeclarations::IDObjectsPool IDObjects, InactiveObjects;
    //std::vector<char*> SoundFiles;
    std::vector<OgreAL::Sound*> Music;
    
	CommonDeclarations::CollisionObjectsPool CollisionObjects;
	void UpdateCollision(Room *room);
	void UpdateCollision(Array<IAAObject*> *objects);
    void UpdateUpVector();
    CommonDeclarations::RadarObjectsArray RadarObjects;	
	std::vector<TiXmlNode*> Levels;
    std::auto_ptr<AAProfile> GameProfile;	
    std::auto_ptr<TiXmlElement> GameConfig;
    
	bool PlayerIsKilled;
	IAAObject *IDObjectsCache[IDObjectsCacheSize];
        
    Ogre::Vector3 UpVector;
    Stall StallUnit;

	char *ScriptHandlers[1];
	int ScriptHandlersTimeouts[1], ScriptHandlersTimes[1];

    Ogre::MaterialPtr SensorMaterial;
    Ogre::Rectangle2D *Sensor;
    float SensorSize;

	char IntroSoundFile[50];
};




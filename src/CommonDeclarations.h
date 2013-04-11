#pragma once
#include <Ogre.h>

//#ifdef MEMORY_POOL
//#include "MemoryPool.h"
//#endif

#include "List.h"
#include "Array.h"
#include "Utilities.h"

#include "HashMap.h"

class AApplication;
class AAObject;
class RoomsManager;
class Player;
class AACamera;
class IAAObject;
class IScenable;
class CollisionModel3D;
struct AAProfile;

//#define SHOW_COLLID_DATA
//#define ROOMS_SHOW_DEBUG_DATA
//#define ROOMS_DEBUG
//#define PATH_FINDER_NODES_DEBUG
//#define PATH_FINDER_DEBUG

#define OBJDESTROY_DEFINITION virtual void Destroy() {delete this;}

#define GUI_RESOURCE_GROUP "GUI"
#define LEVEL_RESOURCE_GROUP "Level"

enum EObjTag {T_UNKNOWN, T_PLAYER, T_CAMERA, T_GENERICAMMO, T_TRIGGER, T_INCUBATOR, T_BILLBOARDAMMO, T_EULERAMMO};

//#define INNER_UPDATE_FPS		30
//#define INNER_UPDATE_INTERVAL_MS float(1000.f/INNER_UPDATE_FPS)

//#define OUTER_UPDATE_FPS		50
//#define OUTER_UPDATE_INTERVAL_MS float(1000.f/OUTER_UPDATE_FPS)

#define PHYSICS_UPDATE_FPS		16
const float PHYSICS_UPDATE_INTERVAL_MS =  float(1000.f/PHYSICS_UPDATE_FPS);

#define TARGET_FPS		60
const float TARGET_UPDATE_INTERVAL_MS = float(1000.f/TARGET_FPS);

namespace OgreAL {
	class SoundManager;
}


namespace CommonDeclarations
{
    const int MaxDistForRadar = 30000; // in ogre units
    const int SqMaxDistForRadar = MaxDistForRadar*MaxDistForRadar; // in ogre units
	const int RadarObjectsCount = 30;
		
	//typedef std::hash_map< IDKeyType, IAAObject*,std::hash_compare <IDKeyType, greater_int > > IDObjectsPool;
	typedef std::map< int, IAAObject* > IDObjectsPool;
    //typedef std::hash_map< AAUtilities::StrKeyType, CollisionModel3D*,std::hash_compare <AAUtilities::StrKeyType, AAUtilities::greater_str > > CollisionObjectsPool;
	typedef AAHashMap<CollisionModel3D*> CollisionObjectsPool;
    typedef Array<Ogre::Vector3> RadarObjectsArray;
	typedef List<IAAObject*> ObjectsPool;

	AApplication *GetApplication();
	Ogre::SceneManager *GetSceneManager();
	char *GenGUID();
	void DeleteObjectRequest(IAAObject *object);
	void AddObjectRequest(IAAObject *object);
	void RegisterObject(IAAObject *object);
	float GetFPS();
	RoomsManager *GetRoomManager();
	void RemoveIDObject(int id);
	void AddIDObject(IAAObject *obj);
	IAAObject *GetIDObject(int id);
	int GetNewID();
	Player *GetPlayer();	
	AACamera *GetCamera();    
    OgreAL::SoundManager *GetSoundManager();
	CollisionObjectsPool *GetCollisionObjectsPool();
    //const RadarObjectsArray *GetRadarObjects();
	void SetMusicGain(float gain);
	float GetMusicGain();
	const char* GetCurrentProfileName();
	void SetCurrentProfileName(const char* profile_name);
	const char* GetLevelPath();
	AAProfile *GetProfile();

	void PlayerKilled();
	void BlockPlayer(bool on);
	void ActivateObject(int id, bool on);
	void SetScriptHandler(int handler_num, char *handler_name, int timeout);
	//CommonDeclarations::IDObjectsPool *GetIDObjects();
	CommonDeclarations::ObjectsPool *GetObjects();
    Ogre::Vector3 GetUpVector();
    void SetAutoPilotActive( bool on );
    void SetSensorVisible( bool on );
	void PlayIntroMusic(bool on);
    TiXmlElement* GetGameConfig();
	CommonDeclarations::ObjectsPool *GetEditableObjects();
};

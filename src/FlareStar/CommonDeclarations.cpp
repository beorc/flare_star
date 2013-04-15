#include "StdAfx.h"
#include <OgreAL.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "CommonDeclarations.h"
#include "Core.h"
#include "ScriptManager.h"
#include "IAAObject.h"

Core* CoreInstance = Core::GetInstance();

AApplication *CommonDeclarations::GetApplication() 
{
	return CoreInstance->GetApplication();
}

Ogre::SceneManager *CommonDeclarations::GetSceneManager() 
{
	return CoreInstance->GetSceneManager();
}

char *CommonDeclarations::GenGUID()
{
	return CoreInstance->GenGUID();
}

void CommonDeclarations::DeleteObjectRequest(IAAObject *object)
{
	CoreInstance->DeleteObjectRequest(object);
}

void CommonDeclarations::AddObjectRequest(IAAObject *object)
{
	CoreInstance->AddObjectRequest(object);
}

void CommonDeclarations::RegisterObject(IAAObject *object)
{
	CoreInstance->RegisterObject(object);
}

float CommonDeclarations::GetFPS()
{
	return CoreInstance->GetFPS();
}

RoomsManager *CommonDeclarations::GetRoomManager()
{
	return CoreInstance->GetRoomManager();
}

void CommonDeclarations::RemoveIDObject(int id)
{
	CoreInstance->RemoveIDObject(id);
}

void CommonDeclarations::AddIDObject(IAAObject *obj)
{
	CoreInstance->AddIDObject(obj);
}

IAAObject *CommonDeclarations::GetIDObject(int id)
{
	return CoreInstance->GetIDObject(id);
}

int CommonDeclarations::GetNewID()
{
	return CoreInstance->GetNewID();
}

AACamera *CommonDeclarations::GetCamera()
{
	return CoreInstance->GetCamera();
}

OgreAL::SoundManager *CommonDeclarations::GetSoundManager()
{
    return CoreInstance->GetSoundManager();
}

Player *CommonDeclarations::GetPlayer()
{
	return CoreInstance->GetPlayer();
}

CommonDeclarations::CollisionObjectsPool *CommonDeclarations::GetCollisionObjectsPool()
{
	return CoreInstance->GetCollisionObjectsPool();
}

void CommonDeclarations::SetMusicGain(float gain)
{
	CoreInstance->SetMusicGain(gain);
}

float CommonDeclarations::GetMusicGain()
{
	return CoreInstance->GetMusicGain();
}
//const CommonDeclarations::RadarObjectsArray *CommonDeclarations::GetRadarObjects()
//{
//    return CoreInstance->GetRadarObjects();
//}

const char* CommonDeclarations::GetCurrentProfileName()
{
	return CoreInstance->GetCurrentProfileName();
}

void CommonDeclarations::SetCurrentProfileName(const char* profile_name)
{
	CoreInstance->SetCurrentProfileName(profile_name);
}

const char* CommonDeclarations::GetLevelPath()
{
	return CoreInstance->GetLevelPath();
}

AAProfile *CommonDeclarations::GetProfile()
{
	return CoreInstance->GetProfile();
}

void CommonDeclarations::PlayerKilled()
{
}

void CommonDeclarations::BlockPlayer(bool on)
{
	CoreInstance->BlockPlayer(on);
}

void CommonDeclarations::ActivateObject(int id, bool on)
{
	CoreInstance->ActivateObject(id, on);
}

void CommonDeclarations::SetScriptHandler(int handler_num, char *handler_name, int timeout)
{
	CoreInstance->SetScriptHandler(handler_num, handler_name, timeout);
}

//CommonDeclarations::IDObjectsPool *CommonDeclarations::GetIDObjects()
//{
//	return CoreInstance->GetIDObjects();
//}

CommonDeclarations::ObjectsPool *CommonDeclarations::GetObjects()
{
	return CoreInstance->GetObjects();
}

Ogre::Vector3 CommonDeclarations::GetUpVector()
{
    return CoreInstance->GetUpVector();
}

void CommonDeclarations::SetAutoPilotActive( bool on )
{
    CoreInstance->SetAutoPilotActive(on);
}

void CommonDeclarations::SetSensorVisible( bool on )
{
    CoreInstance->SetSensorVisible(on);
}

void CommonDeclarations::PlayIntroMusic(bool on)
{
	CoreInstance->PlayIntroMusic(on);
}

TiXmlElement* CommonDeclarations::GetGameConfig()
{
    return CoreInstance->GetGameConfig();
}

CommonDeclarations::ObjectsPool *CommonDeclarations::GetEditableObjects()
{
	return CoreInstance->GetEditableObjects();
}


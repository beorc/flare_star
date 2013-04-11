#include "StdAfx.h"
#include <objbase.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\core.h"
#include "Player.h"

#include "Incubator.h"
#include "ScriptManager.h"


//#include "Debugging.h"
#include "AApplication.h"
#include "AACamera.h"

#include "RoomsManager.h"
#include "Utilities.h"
#include "IAAObject.h"
#include "IScriptable.h"
#include "Container.h"
#include "ActivationTrigger.h"
#include "Configuration.h"
#include "Room.h"

#include "IRoomable.h"
#include "Collidable.h"
#include "IPhysical.h"
#include "CollisionDetection.h"
#include "coldet.h"
#include "CollisionResponse.h"

#include "GuiSystem.h"
#include "GUIRadarLayout.h"
#include "AACommandsFrameListener.h"

//#include "AmmoFactory.h"
#include "Profile.h"
#include "StringResources.h"
#include "ActivationTrigger.h"
#include "ObjectsLibrary.h"
#include "Descriptable.h"
#include "HUDCentre.h"
#include "BillboardSubEffect.h"

#include "Editor\EditorFrameHandler.h"
#include "Editor\EditorFrameListener.h"

//Core *Core::Instance = NULL;

Core::Core(void):
OgreApplication(NULL),
OgreFrameListener(NULL),
PlayersObject(NULL),
SceneManager(NULL),
CreatingNode(NULL),
LevelRoomManager(new RoomsManager),
SoundManager(NULL),
BackGroundSound(NULL),
CreatingObject(NULL),
DropCreatedEntity(false),
DropCreatedNode(false),
Objects(100000),
PlayerIsKilled(false),
CollisionObjects(100),
ObjectsToDel(1000),
ObjectsToAdd(1000),
UpVector(Ogre::Vector3::UNIT_Y),
Sensor(NULL),
SensorSize(0.01),
IntroSound(NULL)
{
	GameProfile.reset(new AAProfile);
	memset(IntroSoundFile,0,sizeof(IntroSoundFile));
	LoadGameConfig();
	memset(ScriptHandlers,0,sizeof(ScriptHandlers));
}

Core::~Core(void)
{
	//MemPool::MemoryPool::Close();
}

Core *Core::GetInstance()
{
	static Core *Instance = new Core;
	return Instance;
}

int Core::Init()
{	
    //ConfigurationInstance = Configuration::GetInstance();
    //ConfigurationInstance->Load();

    StringResources::GetInstance()->Init();
	//AmmoFactory::GetInstance()->Init();
	//PhysicsEngineInstance = PhysicsEngine::GetInstance();
	//PhysicsEngineInstance->Init();

    SoundManager = new OgreAL::SoundManager();

	RadarObjects.Resize(CommonDeclarations::RadarObjectsCount);

	GameProfile->Load();
	
	StallUnit.Load();
	GUISystem::GetInstance()->GetRadar()->Load();

	AAUtilities::SRand(time(NULL)*clock());
	srand(time(NULL)*clock());

    Sensor = new Ogre::Rectangle2D(TRUE);
    Sensor->setMaterial("SensorMaterial");    
    Sensor->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY-3);
    Sensor->setCorners(-SensorSize, SensorSize, SensorSize, -SensorSize);

	if (strlen(IntroSoundFile)>0)
	{
		IntroSound = SoundManager->createSound(IntroSoundFile, IntroSoundFile, true, true);			
		assert(IntroSound);
		IntroSound->setGain(Configuration::GetInstance()->GetMusicVolume()/100.f);
		IntroSound->setRelativeToListener(true);	
	}

	HUDCentre::GetInstance()->Load();
        
	return 0;
}

void Core::Start()
{
    if (Sensor)
        CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(Sensor);
	StallUnit.Init();
	HUDCentre::GetInstance()->Init();
	//GUISystem::GetInstance()->GetRadar()->Load();
	GUISystem::GetInstance()->GetRadar()->Init();
	GUISystem::GetInstance()->SetHudVisible(true);	
	//GUISystem::GetInstance()->GetRadar()->Show();
	PlayersObject->GetNode()->attachObject(SoundManager->getListener());
    MusicFinished(NULL);
	
	Camera->SetRotationDamping(GameProfile->CameraRotationDamping);
    Camera->SetSens(GameProfile->MouseSens);

    //memset(LastTimes,0,sizeof(LastTimes));
	BlockPlayer(false);
		
	ScriptManager::GetInstance()->Call("Start", "i", false, 0);
	//UpdateMusic(2000);
	//OgreApplication->InitGui();
    //std::vector<OgreAL::Sound*>::iterator iPos=Music.begin(), iEnd=Music.end();
    //for (;iPos!=iEnd;++iPos)
    //{   
    //    (*iPos)->setRelativeToListener(true);        
    //}
}

void Core::Reset()
{	
	{
		memset(ScriptHandlersTimeouts,0,sizeof(ScriptHandlersTimeouts));
		memset(ScriptHandlersTimes,0,sizeof(ScriptHandlersTimes));		
		size_t count = sizeof(ScriptHandlers)/sizeof(char*);
		for (size_t i=0;i<count;++i)
		{
			if (ScriptHandlers[i])
			{
				delete [] ScriptHandlers[i];
				ScriptHandlers[i] = NULL;
			}		
		}
	}	

	{	
		ActivationTrigger *trigger;
		for (TriggersContainer::ListNode *pos = ActivationTriggers.GetBegin();pos!=NULL;pos=pos->Next)
		{
			trigger = pos->Value;			
			trigger->ApplyIn(NULL);
		}
		ActivationTriggers.Clear();
		
		IAAObject *object;
		for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin();pos!=NULL;pos=pos->Next)
		{
			object = pos->Value;
			object->Unregister();
			if (object->GetTag()!=T_CAMERA)
			{				
				object->Destroy();
			}			
		}
		Objects.Clear();

		CommonDeclarations::IDObjectsPool::iterator iPos=InactiveObjects.begin(), iEnd=InactiveObjects.end();
		for (;iPos!=iEnd;++iPos)
		{
			object = iPos->second;
			object->Destroy();
		}
		InactiveObjects.clear();
		
	}

	BillboardSubEffect::Reset();
	
	LevelRoomManager->ClearRooms();  
	
	{
		/*CommonDeclarations::CollisionObjectsPool::iterator iPos=CollisionObjects.begin(), iEnd=CollisionObjects.end();
		AAUtilities::StrKeyType name;
		for (;iPos!=iEnd;++iPos)
		{
			name = iPos->first;
			delete [] name;
			deleteCollisionModel3D(iPos->second);
		}
		CollisionObjects.clear();*/
		Array<CollisionModel3D*> *data = CollisionObjects.GetData();
		for (size_t i=0;i<data->Size;++i)
		{
			if (data->Data[i])
				delete data->Data[i];
		}
		CollisionObjects.Clear();
	}	

	memset(IDObjectsCache, 0, sizeof(IDObjectsCache));
	IDObjects.clear();

    std::vector<OgreAL::Sound*>::iterator iPos = Music.begin(), iEnd = Music.end();

    //OgreAL::SoundManager *manager = CommonDeclarations::GetSoundManager();
    for (;iPos!=iEnd;++iPos)
    {
        OgreAL::Sound *sound = *iPos;
        sound->stop();
        //SoundManager->destroySound(sound);
    }
    
    Music.clear();

	if (IntroSound)
	{
		IntroSound->stop();
		//SoundManager->destroySound(IntroSound);
		IntroSound = NULL;
	}

    if (SoundManager)
	    SoundManager->destroyAllSounds();
	BackGroundSound = NULL;	
	

	/*if (ScriptHandlersModule.get())
		ScriptHandlersModule->Close();*/
	ScriptManager::GetInstance()->Reset();
	XMLConfigurable::Reset();
	Descriptable::Reset();

	ObjectsLibrary::GetInstance()->Reset();

	HUDCentre::GetInstance()->Reset();
	
	//ScriptHandlersActive = false;

	//StringResources::GetInstance()->Reset();	
}

void Core::Close()
{
	Reset();
		
	GUISystem::Close();
	if (LevelRoomManager)
		delete LevelRoomManager;  
	if (SoundManager)
		delete SoundManager;
	Camera->Destroy();

	StringResources::GetInstance()->Close();
    StallUnit.Close();

	XMLConfigurable::Close();
	BillboardSubEffect::Close();
	ObjectsLibrary::Close();
	Descriptable::Close();
	Configuration::Close();
	ScriptManager::Close();    
	HUDCentre::GetInstance()->Close();
	
	delete GetInstance();
	//MemPool::MemoryPool::GetGlobalPool().SetCountToDie(true);
}

void Core::DeleteObjectRequest(IAAObject *object)
{	
	//if (object->GetTag()==T_INCUBATOR)
	//	object=object;
	if (object->IsRemoving())
		return;
	if (object->IsVolatile())
	{
		ActivationTrigger *trigger;
		for (TriggersContainer::ListNode *pos = ActivationTriggers.GetBegin();pos!=NULL;pos=pos->Next)
		{
			trigger = pos->Value;			
			bool removed = trigger->RemoveFromActivationObjects(object);
		}
	}	
	object->SetRemoving(true);
	ObjectsToDel.PushBack(object);
}

void Core::SetCamera(AACamera *camera)
{
	Camera = camera;
}

Player *Core::GetPlayer()
{
	return PlayersObject;
}

AACamera *Core::GetCamera()
{
	return Camera;
}

void Core::MusicFinished(OgreAL::Sound *sound)
{
        if (Music.empty() || Configuration::GetInstance()->GetMusicVolume()==0)
            return;
		if (BackGroundSound)
		{
			//if (BackGroundSound->isPlaying())
				//return;
			if (BackGroundSound->isStopped())
				BackGroundSound->play();
			return;
		} else
		{
			float fidx = (Music.size()-1.f)*(rand()/(RAND_MAX+1.f));
            size_t idx = (size_t)(fidx+0.5f);
            BackGroundSound = Music[idx];
            BackGroundSound->play();
		}   
}

inline void Core::AddCreatedObjects()
{
    if (!ObjectsToAdd.IsEmpty())
    {
        IAAObject *object;
        //TempObjectsPool::iterator iPos = ObjectsToAdd.begin(), iEnd = ObjectsToAdd.end();
        for (TempObjectsPool::ListNode *pos = ObjectsToAdd.GetBegin();pos!=NULL;pos = pos->Next)
        {
            object = pos->Value;
            RegisterObject(object);
        }	
        ObjectsToAdd.Clear();
    }
}

void Core::Step(unsigned timeMs)
{	
	int game_state = OgreApplication->GetCurrentGameState();
			
	if (PlayerIsKilled)
	{
		//OgreApplication->SwitchListener();
		OgreApplication->SetCurrentGameState(AApplication::GS_GUI);
		GUISystem::GetInstance()->GetIntroMenu()->Show();
	}

    AddCreatedObjects();

	ActivationTrigger *trigger;
	for (TriggersContainer::ListNode *pos = ActivationTriggers.GetBegin();pos!=NULL;pos=pos->Next)
	{
		trigger = pos->Value;			
		trigger->Step(timeMs);
	}

	for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin();pos!=NULL;pos=pos->Next)		
    {
		IScenable *scen = pos->Value->GetScenable();
        if (scen && scen->GetType()==PT_DYNAMIC) 
        {		
            //if (LastTimes[1]>=OUTER_UPDATE_INTERVAL_MS)
            {
				pos->Value->GetPhysical()->Step(PHYSICS_UPDATE_INTERVAL_MS);				
            }                    
        }       
    }
    
	//if ( LastTimes[2]>=INNER_UPDATE_INTERVAL_MS )
	{
		{			
			RoomsManager::RoomsPool *pool = LevelRoomManager->GetRooms();
			//RoomsManager::RoomsPool::const_iterator iPos=pool->begin(), iEnd=pool->end();
			//for (size_t i=0;i<pool->Size;++i)
			for (RoomsManager::RoomsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
			{
				UpdateCollision(pos->Value);
			}			
		}
		//LastTimes[2]=0;
	}
	
	{
		LevelRoomManager->ClearOuterObjects();
	}
    
    {
        UpdateUpVector();
    }

	{		
		//List<IAAObject*> RayTraceObjects;
		Ogre::Camera *camera =  Camera->GetOgreCamera();
		Ogre::Matrix4 vm = camera->getViewMatrix();
		Ogre::Matrix4 pm = camera->getProjectionMatrix();

        unsigned i = 0;
        IScenable *scen;
		IEquipped *eq = PlayersObject->GetEquipped();
        Ogre::Vector3 pl_pos = PlayersObject->GetPosition(), rel_pos;     
		Ogre::Ray pl_ray = eq->GetSightRay(); //(pl_pos, PlayersObject->GetDirection());

		Ogre::Vector3 sight_sensor_pos(Ogre::Vector3::ZERO);
		//Ogre::Vector3 sight_sensor_pos = pl_ray.getPoint(2000);
		//sight_sensor_pos = vm*sight_sensor_pos;
		//sight_sensor_pos = pm*sight_sensor_pos;

        //int SquaredMaxDist = CommonDeclarations::MaxDistForRadar*CommonDeclarations::MaxDistForRadar;
		//std::pair<bool, Ogre::Real> intersection_res;
		Ogre::Real last_dist = 9999999;
		bool target_found=false;

		//RayTraceObjects.Clear();		
		for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin();pos!=NULL;pos=pos->Next)		
		{	
			pos->Value->Step(timeMs);
            
            scen = pos->Value->GetScenable();
            if (pos->Value != PlayersObject && scen)
            {   
				if (i < RadarObjects.Size && scen->GetType()==PT_DYNAMIC)
				{
					//obj_pos = scen->GetPosition();
					rel_pos = scen->GetPosition() - pl_pos;
					
                    /*if (rel_pos.x<=CommonDeclarations::MaxDistForRadar && 
                        rel_pos.y<=CommonDeclarations::MaxDistForRadar &&
                        rel_pos.z<=CommonDeclarations::MaxDistForRadar)*/
                    if (rel_pos.squaredLength()<=CommonDeclarations::SqMaxDistForRadar)
					{
						RadarObjects.Data[i++] = rel_pos;
					}
				}
				
				if (AApplication::GS_GAME == game_state && pos->Value->GetDestructable())
				{
                    //Ogre::AxisAlignedBox obj_box = scen->GetBoundingBox(true);
                    Ogre::Vector3 obj_pos = scen->GetPosition()/*, obj_extents = obj_box.getHalfSize()*/;
					
					Ogre::Vector3 obj2dpos = obj_pos;
					obj2dpos = vm*obj2dpos;
					obj2dpos = pm*obj2dpos;
					float quad_half_size = 0.15f;

					int sight_dist=0;
					bool in_sight=false;
					if (obj2dpos.z>0 && obj2dpos.z<1 &&
						obj2dpos.x-quad_half_size<sight_sensor_pos.x &&
						obj2dpos.x+quad_half_size>sight_sensor_pos.x &&
						obj2dpos.y-quad_half_size<sight_sensor_pos.y &&
						obj2dpos.y+quad_half_size>sight_sensor_pos.y)
					{
						in_sight = true;						
						sight_dist = (obj_pos-pl_ray.getOrigin()).length();
					}			

					
					//intersection_res = pl_ray.intersects(obj_box);
					//if (intersection_res.first && last_dist > intersection_res.second)
					if (in_sight && last_dist>sight_dist)
					{                        
                        StallUnit.SetPosition(StallUnit.Stalling, obj_pos, StallUnit.StallSize);
						StallUnit.Stalling->setVisible(true);						

						IEquipped *eq = PlayersObject->GetEquipped();
						IPhysical *target_phys = pos->Value->GetPhysical();
						if (target_phys)
						{
							Ogre::Vector3 forestall_pos = eq->CalculateTargetPosition(obj_pos, target_phys->GetLastVelocity()/PHYSICS_UPDATE_INTERVAL_MS);
							//Ogre::AxisAlignedBox forestall_box(forestall_pos.x-obj_extents.x, forestall_pos.y-obj_extents.y, forestall_pos.z-obj_extents.z,
							//	forestall_pos.x+obj_extents.x, forestall_pos.y+obj_extents.y, forestall_pos.z+obj_extents.z);
							
							StallUnit.SetPosition(StallUnit.Forestalling, forestall_pos, StallUnit.ForestallSize);							
							StallUnit.Forestalling->setVisible(true);
						}						
						
						//eq->SetTargetPosition(obj_pos, intersection_res.second);
						//last_dist = intersection_res.second;						
						eq->SetTargetPosition(obj_pos, sight_dist);
						last_dist = sight_dist;
						target_found=true;
					}
				}				
            }			
		}
		if (!target_found)
		{
			PlayersObject->GetEquipped()->SetTargetPosition(Ogre::Vector3::ZERO);
			StallUnit.Stalling->setVisible(false);
			StallUnit.Forestalling->setVisible(false);
		}

        for (;i<RadarObjects.Size;++i)
        {
            RadarObjects.Data[i] = CommonDeclarations::MaxDistForRadar;
        }
        ((GUIRadarLayout*)GUISystem::GetInstance()->GetRadar())->Update(RadarObjects);
	}
    
	{
		/*if (ScriptHandlersActive && ScriptHandlersModule.get())
			ScriptHandlersModule->Fire(timeMs);	*/		
		if (ScriptHandlers[ESHT_TIMER])
		{
			ScriptHandlersTimes[ESHT_TIMER]+=timeMs;
			if (ScriptHandlersTimeouts[ESHT_TIMER]<ScriptHandlersTimes[ESHT_TIMER])
			{
				ScriptHandlersTimes[ESHT_TIMER] = 0;
				ScriptManager::GetInstance()->Call(ScriptHandlers[ESHT_TIMER], "i", false, 0);
			}
		}
	}

	{
		if (LevelRoomManager->GetOuterObjectsNumber()>1)
			UpdateCollision(LevelRoomManager->GetOuterObjects());
	}

	if (!ObjectsToDel.IsEmpty())
	{
		IAAObject *object;
		//TempObjectsPool::iterator iPos = ObjectsToDel.begin(), iEnd = ObjectsToDel.end();
		for (TempObjectsPool::ListNode *pos = ObjectsToDel.GetBegin();pos!=NULL;pos = pos->Next)
		{
			object = pos->Value;
			IScriptable *scr = object->GetScriptable();
            if (scr && scr->GetID()>0)
			{				
				RemoveIDObject(object->GetScriptable()->GetID());
			}
			Objects.Remove(object);			
			object->Unregister();
            object->Destroy();
		}
		ObjectsToDel.Clear();
	}		
   /* if (LastTimes[1]>=OUTER_UPDATE_INTERVAL_MS)
    {		
        LastTimes[1] = 0;
    }*/
}

void Core::AddIDObject(IAAObject *obj)
{
	int id = obj->GetScriptable()->GetID();

	if (id<=0)
	{
		id = GetNewID();
		obj->GetScriptable()->SetID(id);
	}
	if (id<IDObjectsCacheSize)
	{
		IDObjectsCache[id]=obj;
	} else
	{
		std::pair<CommonDeclarations::IDObjectsPool::iterator, bool> f_res = IDObjects.insert(std::make_pair(id, obj));	
		assert(f_res.second);
	}
	

    /*int id = obj->GetScriptable()->GetID();
    CommonDeclarations::IDObjectsPool::iterator iRes = IDObjects.find(id);
    if (iRes==IDObjects.end()) 
    {
        if (id<=0)
        {
            id = GetNewID();
            obj->GetScriptable()->SetID(id);
        }
        IDObjects.insert(std::pair<CommonDeclarations::IDKeyType,IAAObject*>(id,obj));
    } else
	{
		assert(false);
	}*/
}

void Core::RemoveIDObject(int id)
{    
	if (id<IDObjectsCacheSize)
	{
		IDObjectsCache[id]=NULL;
	} else
	{
		CommonDeclarations::IDObjectsPool::iterator iRes = IDObjects.find(id);
		if (iRes!=IDObjects.end()) 
		{
			IDObjects.erase(iRes);
		}		
	}
	{
		CommonDeclarations::IDObjectsPool::iterator iRes = InactiveObjects.find(id);
		if (iRes!=InactiveObjects.end()) 
		{
			InactiveObjects.erase(iRes);
		}
	}
}

int Core::GetNewID()
{
    CommonDeclarations::IDObjectsPool::iterator iRes;
    int id;
    do {        
        id = 100000 + rand() % 10000;
        iRes = IDObjects.find(id);
    } while (iRes!=IDObjects.end());
    
    return id;    
}

IAAObject *Core::GetIDObject(int id)
{
	if (id<IDObjectsCacheSize) {
		return IDObjectsCache[id];
	} else
	{
		CommonDeclarations::IDObjectsPool::iterator iRes = IDObjects.find(id);
		if (iRes==IDObjects.end()) 
		{
			return NULL;
		}
		return iRes->second;
	}
	return NULL;
}

//int Core::LoadScene(OgreMax::OgreMaxScene *loader)
//{	
//	if (0!=Init())
//		return 1;
//	return 0;
//}

bool Core::processMouseInput(const OIS::Mouse *mouse)
{
	using namespace OIS;

	// Rotation factors, may not be used if the second mouse button is pressed
	// 2nd mouse button - slide, otherwise rotate
	const MouseState &ms = mouse->getMouseState();
	if( ms.buttonDown( MB_Left ) )
	{		
		PlayersObject->SetShooting(true);
	} else
	{
		PlayersObject->SetShooting(false);
	}
	
	return true;
}

bool Core::processKeyInput(OIS::Keyboard *keyboard)
{   
	Vector3 AccelerationOn;

	AccelerationOn.z=0;

	if(keyboard->isKeyDown(OIS::KC_UP) || keyboard->isKeyDown(OIS::KC_W) )	
		AccelerationOn.z+=-1;
	
	if(keyboard->isKeyDown(OIS::KC_DOWN) || keyboard->isKeyDown(OIS::KC_S) )		
		AccelerationOn.z+=1;			

	AccelerationOn.x=0;
	if(keyboard->isKeyDown(OIS::KC_RIGHT) || keyboard->isKeyDown(OIS::KC_D) )
		AccelerationOn.x+=1;
	
    if(keyboard->isKeyDown(OIS::KC_LEFT) || keyboard->isKeyDown(OIS::KC_A) )
		AccelerationOn.x+=-1;
	
	AccelerationOn.y=0;

	if(keyboard->isKeyDown(OIS::KC_E))
		AccelerationOn.y+=1;
	
	if(keyboard->isKeyDown(OIS::KC_Q))
		AccelerationOn.y+=-1;

	/*if(keyboard->isKeyDown(OIS::KC_H))
		PlayersObject->GetEquipped()->Drop(false);*/
	if (!PlayersObject->GetAutoPilotActive())
		PlayersObject->GetPhysical()->SetAcceleration(AccelerationOn);

	return true;
}

char* Core::GenGUID()
{
	GUID guid;
	CoCreateGuid(&guid);
	wchar_t wstr[100];

	int ret = StringFromGUID2(guid, wstr, 100);
			
	char* ascii = new char[ret];
	wcstombs( ascii, wstr, ret );

	/*char *buffer = new char[50];
	itoa(,buffer,10);*/
	
	return ascii;
}

void Core::CreatedEntity(const OgreMax::OgreMaxScene* scene, Ogre::Entity* entity) 
{
	//CreatingEntity = entity;
	if (CreatingObject)
		CreatingObject->SetEntity(entity);
}

void Core::StartedCreatingNode(const OgreMax::OgreMaxScene* scene, Ogre::SceneNode* node)
{
	DropCreatedNode=false;
	DropCreatedEntity=false;
	CreatingNode = node;
}

void Core::FinishedCreatingNode(const OgreMax::OgreMaxScene* scene, Ogre::SceneNode* node)
{
	if (CreatingNode && DropCreatedNode)
		SceneManager->destroySceneNode(CreatingNode->getName());	
	/*if (CreatingEntity && DropCreatedEntity)
		SceneManager->destroyEntity(CreatingEntity);*/	
	CreatingNode = NULL;
	//CreatingEntity = NULL;
}

void Core::HandleObjectExtraData(OgreMax::Types::ObjectExtraDataPtr objectExtraData) 
{	
	if (objectExtraData->userData.empty())
		return;
	TiXmlDocument *xml = new TiXmlDocument();	
		
	xml->Parse(objectExtraData->userData.c_str());
	if (xml->Error())
	{        
		StringUtil::StrStreamType errorMessage;
		errorMessage << "There was an error with the scene file";

		OGRE_EXCEPT
			(
			Exception::ERR_INVALID_STATE,
			errorMessage.str(), 
			"Core::HandleObjectExtraData"
			);
	}

	TiXmlElement *root = xml->FirstChildElement();	
	assert( root );
	bool res = ParseOne(root);

	// delete the xml parser after usage
	delete xml;

	return;
}

void Core::CreatePlayer()
{
	PlayersObject = new Player;
	//MALLOC(PlayersObject,Player,);	
	assert(GameProfile.get());
	assert(GameProfile->PlayersProfile);
	if (GameProfile.get() && GameProfile->PlayersProfile)
		PlayersObject->Parse(GameProfile->PlayersProfile->ToElement());
	
	PlayersObject->SetTag(T_PLAYER);
	PlayersObject->InitNode(NULL);
    
	AddObjectRequest(PlayersObject);	
}

bool Core::ParseOne(TiXmlElement* xml)
{		
	CreatingObject = NULL;
	bool res=false;
	if (xml == 0)
		return false; // file could not be opened

	const char *str;

	str = xml->Value();

	if (str!=NULL && strcmp("player",str)==0)
	{			
		if (CreatingNode)			
		{			
            if (PlayersObject->GetInitialPosition().isZeroLength())
			    PlayersObject->SetInitialPosition(CreatingNode->getPosition());			
			DropCreatedNode = true;
			DropCreatedEntity = true;
		}		
		res = true;
	}
	else
		if (str!=NULL && strcmp("node",str)==0)
		{
			//str = xml->Attribute("name");
			str = xml->Attribute("class");
			if (strcmp(str,"SceneObject")==0) 
			{
				SceneObject *obj = new SceneObject();
				
				//MALLOC(obj,SceneObject,);
				obj->Parse(xml);
				obj->SetInitialPosition(CreatingNode->getWorldPosition());						
				obj->SetNode(CreatingNode);
				//obj->SetEntity(CreatingEntity);
				CreatingObject = obj;
				AddObjectRequest(obj);
				obj->SetEditable(true);
			} else
				if (strcmp(str,"Incubator")==0) 
				{
					Incubator *incub = new Incubator;					
					//MALLOC(incub,Incubator,);
					incub->Parse(xml);
					if (CreatingNode)
					{
						incub->SetPosition(CreatingNode->getPosition());							
						DropCreatedNode = true;						
					}	
					/*if (CreatingEntity)
					{						
						DropCreatedEntity = true;
					}	*/
					AddObjectRequest(incub);
					incub->SetEditable(true);
				}	
			res = true;
		} else
			if (strcmp(str,"trigger")==0) 
			{
				str = xml->Attribute("class");
				if (strcmp(str,"container")==0)
				{
					Container *cont = new Container;
					//MALLOC(cont,Container,);
					cont->Parse(xml);
					if (CreatingNode)
					{						
						cont->SetNode(CreatingNode);
						cont->SetInitialPosition(CreatingNode->getWorldPosition());
						cont->SetTriggersPosition(CreatingNode->getWorldPosition());
						//DropCreatedNode = true;						
					}	
					/*if (CreatingEntity)
					{						
					DropCreatedEntity = true;
					}	*/
					
					AddObjectRequest(cont);
					cont->SetEditable(true);
				}
			}
		

		return res;
}

inline void Core::RegisterObject(IAAObject *object)
{
	if (object->GetRegistered())
		return;
	object->Register();
	if (object->GetActive())
		Objects.PushBack(object);	
	else
	{
		IScriptable *scr = object->GetScriptable();
		if (scr && scr->GetID()>0)
		{			
			InactiveObjects.insert(std::make_pair(scr->GetID(),object));			
		}		
	}
}

void Core::AddObjectRequest(IAAObject *object)
{
	ObjectsToAdd.PushBack(object);
}

float Core::GetFPS()
{
    float fps=30.f;
    if (OgreFrameListener)
    {
        fps = OgreFrameListener->GetFPS();
        if (fps==0)
            fps=30.f;
    }
    return fps;
}

void Core::SetFrameListener(AAFrameListener *listener)
{
    OgreFrameListener=listener;
}

void Core::SetMusicGain(float gain)
{
	std::vector<OgreAL::Sound*>::iterator iPos=Music.begin(), iEnd=Music.end();
	for (;iPos!=iEnd;++iPos)
	{
        /*if ((*iPos)->isPlaying())
        {
            if (gain>0)
                (*iPos)->setGain(gain);
            else
                (*iPos)->stop();
        } else
        {
            if (gain>0)
            {
                (*iPos)->setGain(gain);
                (*iPos)->play();
            }
        }*/
		if (gain>0)
		{
			(*iPos)->setGain(gain);
			MusicFinished(NULL);
		}
		else
		{
			if ((*iPos)->isPlaying())
				(*iPos)->stop();
		}
	}
	if (IntroSound)
		IntroSound->setGain(gain);
}

float Core::GetMusicGain() const
{
	if (BackGroundSound)
	{
		return BackGroundSound->getGain();		
	}
	return 0;
}

bool Core::LoadLevel(const char* filename)
{
	//Open the file
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	assert(!stream.isNull());
	if (stream.isNull())
	{		
		StringUtil::StrStreamType errorMessage;
		errorMessage << "Could not open xml file: " << filename;

		OGRE_EXCEPT
			(
			Exception::ERR_FILE_NOT_FOUND,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
			);
	}

	//Get the file contents
	//String data = stream->getAsString();

	// Read the entire buffer	
	size_t Size = stream->size();

	char* pBuf = new char[Size+1];
	// Ensure read from begin of stream
	stream->seek(0);
	stream->read(pBuf, Size);
	pBuf[Size] = '\0';

	//Parse the XML document

	TiXmlDocument *xml = new TiXmlDocument();	
	xml->Parse(pBuf);

	delete [] pBuf;
	stream.setNull();
	assert(!xml->Error());
	if (xml->Error())
	{
		StringUtil::StrStreamType errorMessage;
		errorMessage << "There was an error with the xml file: " << filename;

		OGRE_EXCEPT
			(
			Exception::ERR_INVALID_STATE,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
			);
		return false;
	}

	TiXmlElement *root = xml->FirstChildElement();	
	assert( root );
		
	if (NULL == root)
		return false; // file could not be opened

	// parsing

	const char *str;
	str = root->Value();
	assert(strcmp(str,"level")==0);
	
	TiXmlElement *xml_element = 0;

	xml_element = root->FirstChildElement("objects_library");
	if (xml_element)
	{		
		ObjectsLibrary::GetInstance()->Parse(xml_element);
	}

	xml_element = root->FirstChildElement("rooms");	
	assert(xml_element);

	LevelRoomManager->Parse(xml_element);

	xml_element = root->FirstChildElement("scripts");	
	assert(xml_element);

	ScriptManager::GetInstance()->Parse(xml_element);

    /*xml_element = root->FirstChildElement("script_handlers");	
    
    if (xml_element)
    {
        ScriptHandlersModule.reset(new ScriptHandlersManager);
        ScriptHandlersModule->Parse(xml_element);
    }*/

	/*xml_element = root->FirstChildElement("effects");	
	assert(xml_element);

	EffectsManager::GetInstance()->Parse(xml_element);*/

	xml_element = root->FirstChildElement("scenes");	
	assert(xml_element);

	ParseScenes(xml_element);

    xml_element = root->FirstChildElement("sounds");

    if (xml_element)
    {
       List<char *>* res = AAUtilities::ParseStrings(xml_element, "sound", "file");
        //if (res.get())
        //    SoundFiles = *res.get();
        //std::vector<char *>::iterator iPos=res->begin(), iEnd=res->end();
        for (List<char*>::ListNode *pos = res->GetBegin();pos!=NULL;pos=pos->Next)
        {
            char *file = pos->Value;
            OgreAL::Sound *sound = SoundManager->createSound(file, file, false, false);            
			delete [] file;			
        }
		delete res;
    }

    xml_element = root->FirstChildElement("music");

    if (xml_element)
    {
        List<char *>* res = AAUtilities::ParseStrings(xml_element, "sound", "file");
        
        for (List<char*>::ListNode *pos = res->GetBegin();pos!=NULL;pos=pos->Next)
        {			
            char *file = pos->Value;
            OgreAL::Sound *sound = SoundManager->createSound(file, file, true, true);
			delete [] file;			
            assert(sound);
            sound->setGain(Configuration::GetInstance()->GetMusicVolume()/100.f);
            sound->setRelativeToListener(true);
            sound->addSoundFinishedHandler<Core>(this, &Core::MusicFinished); // !!!
            Music.push_back(sound);
        }
		delete res;
    }
    
    //
    AddCreatedObjects();
    xml_element = root->FirstChildElement("triggers");

    if (xml_element)
    {
        xml_element = xml_element->FirstChildElement("trigger");

        while (xml_element)
        {
            str = xml_element->Attribute("class");
            assert(strcmp(str,"ActivationTrigger")==0);
            if (strcmp(str,"ActivationTrigger")==0)
            {
                ActivationTrigger *cont = new ActivationTrigger;
                //MALLOC(cont,ActivationTrigger,);
                cont->Parse(xml_element);                
                cont->Register();
				cont->ApplyOut(NULL);
                ActivationTriggers.PushBack(cont);
            }
            xml_element = xml_element->NextSiblingElement("trigger");
        }
    }
    //
    
	// delete the xml parser after usage
	delete xml;
    

	Descriptable::Init();
	//if (NULL==PlayersObject)
	//{
	//	//PlayersObject = new Player;
	//	MALLOC(PlayersObject,Player,);
	//	//((XMLConfigurable*)PlayersObject)->Parse("player.xml");	
	//}

	//RegisterObject(PlayersObject);
	return true;
}

bool Core::ParseScenes(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	
	str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}

	str = xml->Value();
	assert(strcmp(str,"scenes")==0);

	TiXmlElement *xml_element = 0;
	xml_element = xml->FirstChildElement("scene");	
	while (xml_element)
	{		        
		str = xml_element->Attribute("file");
		OgreApplication->LoadScene(str);	
		
		xml_element = xml_element->NextSiblingElement("scene");
	}
	return true;
}

RoomsManager *Core::GetRoomManager()
{
	return LevelRoomManager;
}

OgreAL::SoundManager *Core::GetSoundManager()
{
    return SoundManager;
}

CommonDeclarations::CollisionObjectsPool *Core::GetCollisionObjectsPool()
{
	return &CollisionObjects;
}

inline void Core::UpdateCollision(Room *room)
{	
	Room::ChildRoomsPool *rooms = room->GetChildRooms();
	Room::ObjectsPool *objects;

	Room::ObjectsPool::ListNode *with;
	
	Room::ObjectType *object, *o1, *o2;

	Ogre::AxisAlignedBox box1, box2;
		
	for (size_t i=0;i<rooms->Size;++i)
	{
		objects = rooms->Data[i].GetChildObjects(); //iPosRoom->GetChildObjects();

		for (Room::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
		{					
			object = pos->Value;

			if (NULL==object->GetScenable())
				continue;

            //with=pos;
			with=pos->Next;
			box1 = object->GetBoundingBox(true);

			for (;with;with=with->Next)
			{			
				o1=object;
				o2=with->Value;

				assert(o1!=o2);

				if (NULL==o2->GetScenable())
					continue;
				if (o1->GetScenable()->GetType()!=PT_DYNAMIC && o2->GetScenable()->GetType()!=PT_DYNAMIC)
					continue;
				if (NULL==o1->GetCollidable() || NULL==o2->GetCollidable())
					continue;				
				if (o1->GetCollidable()->IsInCollisions(o2->GetCollidable()))
					continue;
				
				box2 = o2->GetBoundingBox(true);
				if (box1.intersects(box2)) 				
				{   
                    bool collide= o1->GetCollidable()->Collide(o2->GetCollidable());

                    if (collide)
                    {	
                        CollisionModel3D *actual_model = o1->GetCollidable()->GetCollisionDetection()->GetActualCollisionModel(); //CollisionDetectionModule->GetActualCollisionModel();
						PhysObjDescriptor o1_descr = o1->GetPhysical()->GetDescriptor(),
											o2_descr = o2->GetPhysical()->GetDescriptor();

						if (PT_DYNAMIC == o2->GetScenable()->GetType())			
                            o1->GetPhysical()->GetCollisionResponse()->Response(&o2_descr, actual_model, true);
                        if (PT_DYNAMIC == o1->GetScenable()->GetType())			
                            o2->GetPhysical()->GetCollisionResponse()->Response(&o1_descr, actual_model, false);
						
						o1->GetPhysical()->SetDescriptor(o1_descr);
						o2->GetPhysical()->SetDescriptor(o2_descr);

                        o1->GetCollidable()->AddCollision(o2->GetCollidable());						
                        o2->GetCollidable()->AddCollision(o1->GetCollidable());
                    }
                    
				}					
			}		

		}			
	}					
}

inline void Core::UpdateCollision(Array<IAAObject*> *objects)
{
	IAAObject *object, *o1, *o2;
	Ogre::AxisAlignedBox box1, box2;

	for (size_t i=0;i<objects->Size;++i)
	{
		object = objects->Data[i];

		if (NULL==object)
			break;

		if (NULL==object->GetScenable())
			continue;
		
		box1 = object->GetBoundingBox(true);

		for (size_t j=i+1;j<objects->Size;++j)
		{			
			o1=object;
			o2=objects->Data[j];
			
			if (NULL==o2)
				break;

			if (NULL==o2->GetScenable())
				continue;
			if (o1->GetScenable()->GetType()!=PT_DYNAMIC && o2->GetScenable()->GetType()!=PT_DYNAMIC)
				continue;
			if (NULL==o1->GetCollidable() || NULL==o2->GetCollidable())
				continue;				
			if (o1->GetCollidable()->IsInCollisions(o2->GetCollidable()))
				continue;

			box2 = o2->GetBoundingBox(true);
			if (box1.intersects(box2)) 				
			{   
				bool collide= o1->GetCollidable()->Collide(o2->GetCollidable());

				if (collide)
				{	
					CollisionModel3D *actual_model = o1->GetCollidable()->GetCollisionDetection()->GetActualCollisionModel(); //CollisionDetectionModule->GetActualCollisionModel();
					PhysObjDescriptor o1_descr = o1->GetPhysical()->GetDescriptor(),
						o2_descr = o2->GetPhysical()->GetDescriptor();

					if (PT_DYNAMIC == o2->GetScenable()->GetType())			
						o1->GetPhysical()->GetCollisionResponse()->Response(&o2_descr, actual_model, true);
					if (PT_DYNAMIC == o1->GetScenable()->GetType())			
						o2->GetPhysical()->GetCollisionResponse()->Response(&o1_descr, actual_model, false);

					o1->GetPhysical()->SetDescriptor(o1_descr);
					o2->GetPhysical()->SetDescriptor(o2_descr);

					o1->GetCollidable()->AddCollision(o2->GetCollidable());						
					o2->GetCollidable()->AddCollision(o1->GetCollidable());
				}
			}
		}		
	}					
}

bool Core::LoadGameConfig()
{
	TiXmlDocument *xml = new TiXmlDocument("media/common/configs/game.xml");	
	TiXmlElement *root=0, *xml_element = 0;

	bool loadOkay = xml->LoadFile();

	assert(loadOkay);
	if (!loadOkay)
	{		
		return false; // file could not be opened
	}

	root = xml->FirstChildElement();	
	assert( root );

    GameConfig.reset(root->Clone()->ToElement());
	
	xml_element = root->FirstChildElement("levels");	
	if (xml_element)
	{
		xml_element = xml_element->FirstChildElement("level");	
		while (xml_element)
		{
			Levels.push_back(xml_element->Clone());
			xml_element = xml_element->NextSiblingElement("level");
		}
	}	

	xml_element = root->FirstChildElement("stalling");	
	if (xml_element)
	{
		StallUnit.Parse(xml_element);
	}

    xml_element = root->FirstChildElement("sensor");	
    if (xml_element)
    {
        int res;
        res = xml_element->QueryFloatAttribute("size", &SensorSize);        
    }

	xml_element = root->FirstChildElement("intro");	
	if (xml_element)
	{
		const char *str;
		str = xml_element->Attribute("music");
		if (str)
			strcpy(IntroSoundFile, str);
	}

	xml_element = root->FirstChildElement("hud");	
	if (xml_element)
	{
		HUDCentre::GetInstance()->Parse(xml_element);
	}

	delete xml;

	return true;
}

bool Core::SaveProfile()
{
    if (GameProfile.get()) 
    {
        GameProfile->Save();
    }    

	return true;
}

bool Core::LoadProfile()
{    
    GameProfile->Load();
    
	return true;
}

const char* Core::GetLevelPath()
{
	const char *str;
	TiXmlElement *level_elem;	
	level_elem = Levels[GameProfile->CurrentLevel-1]->ToElement();
	str = level_elem->Attribute("path");
	return str;
}

void Core::SetCurrentProfileName(const char* profile_name)
{
	strcpy(GameProfile->ProfileName,profile_name);
}

const char* Core::GetCurrentProfileName()
{
	return GameProfile->ProfileName;
}

AAProfile *Core::GetProfile() const
{
	return GameProfile.get();
}

void Core::PlayerKilled()
{
	PlayerIsKilled=true;
}

CommonDeclarations::ObjectsPool *Core::GetObjects()
{
	return &Objects;
}

void Core::BlockPlayer(bool on)
{
	if (OgreFrameListener)
		OgreFrameListener->SetBlocked(on);
	AACommandsFrameListener::GetInstance()->SetBlocked(on);

	PlayersObject->SetActive(!on);
	Camera->SetActive(!on);
}

bool Core::IncludeObjectToQueue(IAAObject *obj)
{
	bool found=false;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin(); pos!=NULL; pos=pos->Next)
	{
		if (obj==pos->Value)
		{
			found=true;
			break;
		}
	}
	//assert(!found);
	if (!found)
		Objects.PushBack(obj);
	return found;
}

bool Core::ExcludeObjectFromQueue(IAAObject *obj)
{
	bool found=false;	
	
	for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin(); pos!=NULL; pos=pos->Next)
	{
		if (obj==pos->Value)
		{
			found=true;
			break;
		}
	}
	if (found)
		Objects.Remove(obj);
	return found;	
}

void Core::ActivateObject(int id, bool on)
{
	IAAObject *obj = GetIDObject(id);
	if (NULL==obj)
		return;	
	if (on)
	{
		CommonDeclarations::IDObjectsPool::iterator iRes = InactiveObjects.find(id);
		if (iRes!=InactiveObjects.end()) 
		{			
			IncludeObjectToQueue(obj);
			InactiveObjects.erase(iRes);
		}    		
	} else
	{
		if (ExcludeObjectFromQueue(obj))
			InactiveObjects.insert(std::make_pair(id,obj));
	}
	obj->SetActive(on);
}

//void Core::SetScriptHandlersActive(bool on)
//{
//	ScriptHandlersActive=on;
//}

//CommonDeclarations::IDObjectsPool *Core::GetIDObjects()
//{
//	return &IDObjects;
//}

inline void Core::UpdateUpVector()
{
    UpVector = Ogre::Vector3::UNIT_Y;
    if (PlayersObject)
    {
        Ogre::Quaternion obj_orientation = PlayersObject->GetOrientation();
        UpVector = obj_orientation * UpVector;
    }    
}

Ogre::Vector3 Core::GetUpVector() const
{
    return UpVector;
}

void Core::SetScriptHandler(int handler_num, char *handler_name, int timeout)
{
	size_t count = sizeof(ScriptHandlers)/sizeof(char*);
	if (handler_num<(int)count)
	{
		ScriptHandlers[handler_num] = handler_name ? AAUtilities::StringCopy(handler_name) : NULL;
		ScriptHandlersTimeouts[handler_num] = timeout;
	}
}

bool Core::KeyPressed( const OIS::KeyEvent &arg )
{
	if( arg.key == OIS::KC_H )
		PlayersObject->GetEquipped()->Drop(false);

	if( arg.key == OIS::KC_NUMPADENTER )
	{
		EditorFrameListener *listener = EditorFrameListener::GetInstance();
		listener->SetCurrentHandler(NULL);

		EditorFrameHandler *current_handler = new EditorFrameHandler("media//level1//scenes//level.scene");
		current_handler->Init();
			
		//current_handler->Parse(xml);
		
		listener->SetCurrentHandler(current_handler);

		OgreApplication->SetCurrentGameState(AApplication::GS_EDITOR);
	}
	
	if( arg.key == OIS::KC_P )
	{
		if (!PlayersObject->GetAutoPilotActive())
		{
			CommonDeclarations::SetAutoPilotActive(true);
		}
		else
		{
			CommonDeclarations::SetAutoPilotActive(false);
		}

	}
	return false;
}

bool Core::MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

void Core::SetAutoPilotActive( bool on )
{	
	Camera->AutoPilotActive(on);
	//return;
    if (on)
    {
        if (!PlayersObject->GetAutoPilotActive())
        {
            PlayersObject->SetAutoPilotActive(true);
			HUDCentre::GetInstance()->SetAutoPilotActive(true);
        }
    } else
    {
        if (PlayersObject->GetAutoPilotActive())
        {
            PlayersObject->SetAutoPilotActive(false);
			HUDCentre::GetInstance()->SetAutoPilotActive(false);
        }        
    }    
}

void Core::SetSensorVisible( bool on )
{
    if (Sensor) 
    {
        Sensor->setVisible(on);
    }
}

void Core::PlayIntroMusic(bool on)
{	
	if (on)
	{
		if (IntroSound && IntroSound->isStopped())
		{
			IntroSound->play();
		}
	} else
	{
		if (IntroSound && IntroSound->isPlaying())
		{
			IntroSound->stop();
		}
	}
}

TiXmlElement* Core::GetGameConfig() const
{
    return GameConfig.get();
}

ActivationTrigger *Core::GetTriggerByID(int id)
{
	ActivationTrigger *trigger;
	for (TriggersContainer::ListNode *pos = ActivationTriggers.GetBegin();pos!=NULL;pos=pos->Next)
	{
		trigger = pos->Value;
		if (trigger->GetID()==id)
			return trigger;
	}
	return NULL;
}

bool Core::KeyReleased( const OIS::KeyEvent &arg )
{
	return false;
}

bool Core::MouseMoved( const OIS::MouseEvent &arg )
{
	return false;
}

bool Core::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}

CommonDeclarations::ObjectsPool *Core::GetEditableObjects()
{
	CommonDeclarations::ObjectsPool *result = new CommonDeclarations::ObjectsPool;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = Objects.GetBegin();pos!=NULL;pos=pos->Next)
	{
		if (pos->Value->IsEditable())
		{			
			result->PushBack(pos->Value);
		}
	}

	ActivationTrigger *trigger;
	for (TriggersContainer::ListNode *pos = ActivationTriggers.GetBegin();pos!=NULL;pos=pos->Next)
	{
		trigger = pos->Value;			
		const Array<IAAObject*> *activation_objects = trigger->GetActivationObjects();

		for (size_t i=0;i<activation_objects->Size;++i)
		{
			if (activation_objects->Data[i]->IsEditable())
			{			
				result->PushBack(activation_objects->Data[i]);
			}			
		}
	}

	return result;
}




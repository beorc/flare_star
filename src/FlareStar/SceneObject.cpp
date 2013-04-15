#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\sceneobject.h"
#include "CommonDeclarations.h"
#include "EntityMaterialInstance.h"
#include "MasterEffect.h"
#include "EquippedObject.h"
#include "AIModule.h"

#include "Utilities.h"
#include "Roomable.h"
#include "Collidable.h"
#include "Physical.h"
#include "Destructable.h"
#include "Descriptable.h"

#include "RoomsManager.h"

//#include "MeshEffect.h"
//#include "BillboardEffect.h"
#include "ObjectsLibrary.h"
#include "BonusesKeeper.h"
#include "Combat/Combatable.h"

SceneObject::SceneObject():
AAObject(),
Node(NULL),
OgreEntity(NULL),
EntityMaterial(NULL),
Effects(NULL),
InitialRotation(Ogre::Quaternion::IDENTITY),
InvertedInitialOrientation(Ogre::Quaternion::IDENTITY),
SceneObjectInited(false),
Modelname(NULL),
Type(PT_STATIC),
InitialPosition(Ogre::Vector3::ZERO),
//RunEffectsOnCreate(false),
Irx(0),
Iry(0),
Irz(0),
HaveCachedAABB(false),
PrevPosition(Ogre::Vector3::ZERO),
//DescriptableReference(NULL),
InitialDirection(Ogre::Vector3::UNIT_Z),
AiStaff(NULL),
DescriptableID(0)
{
    //memset(LastTimes,0,sizeof(LastTimes));
}

SceneObject::~SceneObject(void)
{
	Ogre::SceneManager *smgr = CommonDeclarations::GetSceneManager();
	if (EntityMaterial)
		delete EntityMaterial;
    if (Node)
    {
        smgr->destroySceneNode(Node->getName());		
    }	
    if (OgreEntity)
    {
        smgr->destroyMovableObject(OgreEntity);		
    }    
	if (Effects)
	{
		IMasterEffect *effect;
		//EffectsPool::iterator iPos=Effects->begin(), iEnd=Effects->end();
		for (EffectsPool::ListNode *pos = Effects->GetBegin();pos!=NULL;pos = pos->Next)
		{
			effect = pos->Value;
			effect->Destroy();
		}
		delete Effects;
	}
	if (Modelname)
		delete [] Modelname;

    //std::vector<char*>::iterator iPos = SoundFiles.begin(), iEnd = SoundFiles.end();
	/*if (SoundFiles.get())
	{
		for (List<char*>::ListNode *pos = SoundFiles->GetBegin();pos!=NULL;pos=pos->Next)
			delete [] pos->Value;	
	}*/
    
    /*if (Sounds.get())
    {
        OgreAL::SoundManager *manager = CommonDeclarations::GetSoundManager();
        for (size_t i =0;i<Sounds->Size;++i)
        {
            Sounds->Data[i]->stop();
            manager->destroySound(Sounds->Data[i]);
        }
    }*/

    if (AiStaff)
    {
        if (AiStaff->TryToDestroy())
            delete AiStaff;
    }
}

Ogre::SceneNode *SceneObject::GetNode() const
{
    return Node;
}

void SceneObject::SetEntity(Ogre::MovableObject *entity)
{
	OgreEntity = entity;
}

void SceneObject::CreateEntityMaterial()
{       
    EntityMaterial = new EntityMaterialInstance((Ogre::Entity*)OgreEntity);       
}

void SceneObject::SetTransparency(Ogre::Real percent)
{
    if (NULL==EntityMaterial)
    {
        CreateEntityMaterial();
        EntityMaterial->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
    }    
    EntityMaterial->setTransparency(percent);    
}

void SceneObject::RemoveEntityMaterial()
{
    delete EntityMaterial;
    EntityMaterial=NULL;
}

EntityMaterialInstance *SceneObject::GetEntityMaterial()
{
    return EntityMaterial;
}

Ogre::Real SceneObject::GetTransparencyPercent()
{
	if (NULL==EntityMaterial)
		return 0.0f;
	
	return EntityMaterial->getTransparency();
}

bool SceneObject::Parse(TiXmlElement* xml)
{	
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	xml = CheckLID(xml);

	const char *str;	
	str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}
	
//	xml = XMLConfigurable::CheckLID(xml);

	AAObject::Parse(xml);

	str = xml->Attribute("modelname");	
	//assert(str);	
	
	if (str)
	{
		if (Modelname)
			delete [] Modelname;
        Modelname = AAUtilities::StringCopy(str);		
	}
	
    int resx, resy, resz;	
    resx = xml->QueryIntAttribute("irx", &Irx);
    //assert(TIXML_SUCCESS==res);
    resy = xml->QueryIntAttribute("iry", &Iry);
    //assert(TIXML_SUCCESS==res);
    resz = xml->QueryIntAttribute("irz", &Irz);
    //assert(TIXML_SUCCESS==res);

    //Init();

    if (TIXML_SUCCESS==resx)
    {
        Quaternion q(Degree(Irx), Vector3::UNIT_X);        
        InitialRotation = q;        
    }
    if (TIXML_SUCCESS==resy)
    {
        Quaternion q(Degree(Iry), Vector3::UNIT_Y);        
        InitialRotation = InitialRotation*q;        
    }
    if (TIXML_SUCCESS==resz)
    {
        Quaternion q(Degree(Irz), Vector3::UNIT_Z);        
        InitialRotation = InitialRotation*q;        
    }

    InvertedInitialOrientation = InitialRotation.Inverse();
			
	TiXmlElement *xml_element = 0;

	xml_element = xml->FirstChildElement("initial_direction");
	//assert(strcmp(str,"effects")==0);
	if (xml_element)
	{
		int x, y ,z;
		resx = xml_element->QueryIntAttribute("x", &x);	
		assert(TIXML_SUCCESS==resx);
		resy = xml_element->QueryIntAttribute("y", &y);	
		assert(TIXML_SUCCESS==resy);
		resz = xml_element->QueryIntAttribute("z", &z);	
		assert(TIXML_SUCCESS==resz);

		if (TIXML_SUCCESS==resx)
		{		
			InitialDirection.x = x;
		}
		if (TIXML_SUCCESS==resy)
		{
			InitialDirection.y = y;
		}
		if (TIXML_SUCCESS==resz)
		{
			InitialDirection.z = z;
		}
	}
	
	xml_element = xml->FirstChildElement("effects");
	//assert(strcmp(str,"effects")==0);
	if (xml_element)
	{
        int val=0, res=0;
        //res = xml_element->QueryIntAttribute("run", &val);        
        //RunEffectsOnCreate = val ? true : false;

		if (Effects)
			delete Effects;
		Effects = new EffectsPool;
		
		MasterEffect *effect;		
		xml_element = xml_element->FirstChildElement("effect");	
		
		while (xml_element)
		{	
            effect = new MasterEffect();
            
            if (effect) 
            {   
                effect->Parse(xml_element);
                Effects->PushBack(effect);
            }
			xml_element = xml_element->NextSiblingElement("effect");
		}
	}

    /*xml_element = xml->FirstChildElement("sounds");

    if (xml_element)
    {
        List<char*>* res = AAUtilities::ParseStrings(xml_element, "sound", "file");
        if (res)
            SoundFiles.reset(res);
    }*/

	str = xml->Attribute("collismodel");
	if (str)
	{
		CollidableModule.reset(new Collidable);
		CollidableModule->SetOwner(this);
		CollidableModule->Parse(xml);
	}

    str = xml->Attribute("physical");
    if (str)
    {
		if (strcmp(str,"notphys_dynamic")==0) {
			Type = PT_NOT_PHYSYCS_DYNAMIC;
		} else
		{
			PhysicalModule.reset(new Physical);
			PhysicalModule->SetOwner(this);
			PhysicalModule->Parse(xml);

			if (strcmp(str,"dynamic")==0)
				Type = PT_DYNAMIC;
		}
    }

    xml_element = xml->FirstChildElement("AI");
    if (xml_element) 
    {				
        AiStaff = new AIModule();
		AiStaff->SetOwner(this);
        AiStaff->Parse(xml_element);
    }

    xml_element = xml->FirstChildElement("equipment");
    if (xml_element) 
    {				
        EquippedStaff.reset(new EquippedObject());
		EquippedStaff->SetOwner(this);
        EquippedStaff->Parse(xml_element);
    }

	xml_element = xml->FirstChildElement("bonuses_keeper");
	if (xml_element) 
	{				
		BonusesModule.reset(new BonusesKeeper());
		BonusesModule->SetOwner(this);
		BonusesModule->Parse(xml_element);
	}

    str = xml->Attribute("roomable");
    if (str && strcmp(str,"on")==0)
    {
        RoomableModule.reset(new Roomable(this));
	RoomableModule->Parse(xml);
    }
		
	xml_element = xml->FirstChildElement("destructable");

	if (xml_element)
	{
		DestructableModule.reset(new Destructable);
		DestructableModule->Parse(xml_element);
	}

	xml_element = xml->FirstChildElement("description");
	if (xml_element) 
	{		
		xml_element->QueryIntAttribute("id", &DescriptableID);
		//DescriptableReference = Descriptable::GetDescription(val);
	}

	xml_element = xml->FirstChildElement("turn_based");
	if (xml_element) 
	{	
		CombatableUnit.reset(new Combatable);
		//xml_element->QueryIntAttribute("id", &DescriptableID);
		//DescriptableReference = Descriptable::GetDescription(val);
	}
		        
	return true;
}

Ogre::Vector3 SceneObject::GetInitialPosition() const
{
	return InitialPosition;
}

Ogre::Quaternion SceneObject::GetInitialRotation() const
{
	return InitialRotation;
}

void SceneObject::SetOrientation(Ogre::Quaternion direction)
{
	//Node->setOrientation(direction * InitialRotation);
	Node->setOrientation(direction);
}

Ogre::Quaternion SceneObject::GetOrientation()
{	
	//return Node->getOrientation() * InvertedInitialOrientation;
	return Node->getOrientation();
}

void SceneObject::SetModel(const char* modelname)
{
	if (NULL==modelname)
		return;
	Modelname = AAUtilities::StringCopy(modelname);
	//Modelname = new char[strlen(modelname)+1];
	//strcpy((char*)Modelname,modelname);
}

void SceneObject::Step(unsigned timeMs)
{
	//LastTimes[1]+=timeMs;
	UpdateBox();
	AAObject::Step(timeMs);
	
	//if (LastTimes[1]>=TARGET_UPDATE_INTERVAL_MS)
	{
		if (Effects)
		{
			IMasterEffect *effect;
			//EffectsPool::iterator iPos=Effects->begin(), iEnd=Effects->end();
			for (EffectsPool::ListNode *pos = Effects->GetBegin();pos!=NULL;pos = pos->Next)
			{
				effect = pos->Value;
				effect->Step(timeMs);
			}
		}
		if (AiStaff)
			AiStaff->StepAI(timeMs);
		if (EquippedStaff.get())
			EquippedStaff->Step(timeMs);  
	}

	ICollidable *collid = GetCollidable();
	if (collid)
		collid->ClearCollisions();

    

	if (Type!=PT_STATIC)
	{
		//LastTimes[0]+=timeMs;
		//if (LastTimes[0]>=INNER_UPDATE_INTERVAL_MS)
		{	
			if ( RoomableModule.get() )
			{
				RoomableModule->SetBox(GetBoundingBox(true));
				CommonDeclarations::GetRoomManager()->UpdateObject(this);				
			}
			//LastTimes[0] = 0;
		}
	}

	if (Type==PT_DYNAMIC) 
    {		
		//if (LastTimes[1]>=OUTER_UPDATE_INTERVAL_MS)
		{
			//GetPhysical()->Step(LastTimes[1]);
			UpdatePosition();
		}
    }

	if (DestructableModule.get())
	{
		if (DestructableModule->GetHealth()<=0)
		{
			if (Killed())
				CommonDeclarations::DeleteObjectRequest(this);
		}
	}    
	
	/*if (LastTimes[1]>=TARGET_UPDATE_INTERVAL_MS)
	{		
		LastTimes[1] = 0;
	}*/
}

bool SceneObject::Killed()
{
	bool res = AAObject::Killed();
	IEquipped *eq = GetEquipped();
	if (eq)
		eq->Drop(true);

	BonusesKeeper *bk = GetBonusesKeeper();
	if (bk)
		bk->Drop(true);
	return res;
}

bool	SceneObject::Init()
{	
	InitEntity();
	InitNode(NULL);
    //InitSounds();

	if (CollidableModule.get())		
		CollidableModule->InitCollisionModel();	
    
    if (EquippedStaff.get())
        EquippedStaff->Init();

    if (PhysicalModule.get())
    {
        PhysicalModule->InitPhysics();		
		PhysicalModule->SetPosition(InitialPosition);
        //if (!RoomableModule.get())
        //    RoomableModule.reset(new Roomable(this));        
    }

    if (RoomableModule.get()) 
    {
		RoomableModule->SetBox(GetBoundingBox(true));
        CommonDeclarations::GetRoomManager()->PutObject(this);
    }

	if (AiStaff)
	{
		AiStaff->Init();
	}

	if (Effects)
	{
		IMasterEffect *effect;
		//EffectsPool::iterator iPos=Effects->begin(), iEnd=Effects->end();
		for (EffectsPool::ListNode *pos = Effects->GetBegin();pos!=NULL;pos = pos->Next)
		{
			effect = pos->Value;
			effect->SetParent(this);
			effect->Init();			
            //effect->InitSounds();			
		}		
	}
    
  //  if (RunEffectsOnCreate)
  //  {
  //      //EffectsPool::iterator iPos = Effects->begin(), iEnd = Effects->end();
		//for (EffectsPool::ListNode *pos = Effects->GetBegin();pos!=NULL;pos = pos->Next)
  //      {
  //          pos->Value->Start();
  //      }
  //  }

	return true;	
}

bool SceneObject::InitEntity()
{
	if (!OgreEntity && Modelname)
	{
		Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();
		
		char *buffer = CommonDeclarations::GenGUID();		
		//char *buffer = new char[50];
		//itoa((int)this,buffer,10);		
		//strcat(buffer,"_entity");
		OgreEntity = SceneMgr->createEntity(buffer, Modelname);
				

		BoundingBoxCache = OgreEntity->getBoundingBox();
		HaveCachedAABB=true;

#ifdef SHOW_COLLID_DATA
		ICollidable *collid = GetCollidable();
		if (collid)
		{
			SetTransparency(0.5f);			
		}		
#endif
		delete [] buffer;
		return true;
	}
	
	return false;
}

bool SceneObject::InitNode(Ogre::SceneNode *parent)
{
	/*if (NULL==OgreEntity )
		return false;*/

	if (NULL==parent)
	{
		Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();
		parent=SceneMgr->getRootSceneNode();
	}
	
	if (NULL==Node)
	{
		Node=parent->createChildSceneNode();
	}

	if (Node)
	{
		Node->setPosition(InitialPosition);
		Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
		if (InitialRotation!=Ogre::Quaternion::IDENTITY)
		{
			q = InitialRotation;
		}
		if (InitialDirection!=Ogre::Vector3::UNIT_Z) 
		{	
			InitialDirection.normalise();

			Ogre::Vector3 up = Ogre::Vector3::UNIT_Y;

			Vector3 xVec = up.crossProduct(InitialDirection);
			xVec.normalise();
			Vector3 yVec = InitialDirection.crossProduct(xVec);
			yVec.normalise();
			Quaternion unitZToTarget = Quaternion(xVec, yVec, InitialDirection);

			Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);
			//Node->setOrientation(q*targetOrientation);
			q = targetOrientation*q;
		}
		Node->setOrientation(q);
		//Ogre::Vector3 base_dir = Ogre::Vector3::NEGATIVE_UNIT_Z;
		//Ogre::Quaternion q = base_dir.getRotationTo(InitialDirection);
		//Node->setOrientation(q);
	}

	if (OgreEntity && Node==OgreEntity->getParentSceneNode())
		return false;

	if (OgreEntity)
		Node->attachObject((MovableObject*)OgreEntity);	

	/*if (Sounds.get())
	{
		SoundsPool::iterator iPos = Sounds->begin(), iEnd = Sounds->end();
		for (;iPos!=iEnd;++iPos)
		{
			Node->attachObject(*iPos);
		}
	}*/
    

#ifdef SHOW_COLLID_DATA
	Node->showBoundingBox(true);	
#endif
	
	return true;
	
}

void SceneObject::UpdateBox()
{	
	if (!HaveCachedAABB && OgreEntity)
	{
		BoundingBoxCache = OgreEntity->getBoundingBox();;
		HaveCachedAABB = true;
	}

	Box = BoundingBoxCache;

	Matrix4 transforms;
	//Node->getWorldTransforms(&transforms);
	transforms.makeTransform(GetPosition(),Node->getScale(),Node->getOrientation());
	Box.transform(transforms);
}

inline Ogre::AxisAlignedBox SceneObject::GetBoundingBox(bool transformed) const
{	
	if (!transformed)
		return BoundingBoxCache;
	
	return Box;
}

void	SceneObject::SetNode(Ogre::SceneNode *node) 
{
	assert(Node==NULL);
	Node=node;
}

void SceneObject::SetParent(IScenable *parent)
{
	if (NULL==Node) 
	{
		InitNode(parent->GetNode());
	} else
	{
		Ogre::Node *newparent = parent->GetNode(), *currparent = Node->getParent();
		if (newparent!=currparent && currparent!=NULL) 
		{
			currparent->removeChild(Node);
			newparent->addChild(Node);
		}			
	}	
}

void SceneObject::SetPosition(Vector3 pos)
{	
    if (PhysicalModule.get())
        PhysicalModule->SetPosition(pos);
    if (Node)
        Node->setPosition(pos);
}

Ogre::Vector3 SceneObject::GetPosition() const
{
	return Node->getPosition();
	
	/*if (PhysicalModule.get())
		return PhysicalModule->GetPosition();
	
    return Node->getPosition();*/
}

void SceneObject::SetDirection(Ogre::Vector3 direction)
{
    Node->setDirection(direction, Ogre::Node::TS_WORLD);
}

Ogre::Vector3 SceneObject::GetScale() const
{
	return Node->getScale();
}

Ogre::Quaternion SceneObject::GetAbsoluteOrientation() const
{
	return Node->getOrientation();
}

void SceneObject::GetBoundingPoints(Ogre::Real Side, Vector3 outPoints[])
{
    Vector3 ExpandDelta(Side,Side,Side);
    Ogre::AxisAlignedBox ExpandedBox = GetBoundingBox(true);
    ExpandedBox.setMinimum(ExpandedBox.getMinimum()-ExpandDelta);
    ExpandedBox.setMaximum(ExpandedBox.getMaximum()+ExpandDelta);

    memcpy(outPoints, ExpandedBox.getAllCorners(),sizeof(Vector3)*8);
    return ;
}

bool SceneObject::RestoreBackupPosition()
{
	IPhysical *phys = GetPhysical();
	char log[100];
	if (phys)
	{		
		Ogre::Vector3 v = phys->GetPosition();
		if (v==PrevPosition) {
			v=v;
		}
		sprintf(log,"1 %f %f %f\n",v.x, v.y, v.z);
		Debugging::Log("rbp",log);
		//phys->Stop();
		//phys->SetPosition(phys->GetPrevPosition()-phys->GetLinVelocity().normalisedCopy()*2*phys->GetRadius());
		phys->SetPosition(PrevPosition);
		Node->setPosition(PrevPosition);
		phys->Stop();
		sprintf(log,"2 %f %f %f\n",PrevPosition.x, PrevPosition.y, PrevPosition.z);
		Debugging::Log("rbp",log);
		return true;
	}
	return false;
}

void SceneObject::SetAiModule(AIModule *module)
{
    AiStaff = module;
}

AIModule *SceneObject::GetAiModule()
{
    return AiStaff;
}

void SceneObject::UpdatePosition()
{    
    if (NULL==PhysicalModule.get())
        return;
    PrevPosition = Node->getPosition();
    Node->setPosition(PhysicalModule->GetPosition());	

    Ogre::Vector3 RotVelocity = PhysicalModule->GetRotVelocity();

    Node->yaw(Ogre::Degree(RotVelocity.y));
    Node->pitch(Ogre::Degree(RotVelocity.x));
    Node->roll(Ogre::Degree(RotVelocity.z));
}

void SceneObject::Register()
{	
    Init();
    
    AAObject::Register();
}

void SceneObject::Unregister()
{
    AAObject::Unregister();
    if (RoomableModule.get())
    {
        RoomableModule->RemoveFromRooms();
        RoomableModule->GetRooms()->Clear();	
    }
}

void SceneObject::SetInitialPosition(Ogre::Vector3 pos)
{
    InitialPosition = pos;
    /*if (PhysicalModule.get())
        PhysicalModule->SetPosition(InitialPosition);*/
}

IPhysical *SceneObject::GetPhysical() const
{
    return PhysicalModule.get();
}

IRoomable *SceneObject::GetRoomable() const
{
    return RoomableModule.get();
}

IScenable *SceneObject::GetScenable() const
{
    return (IScenable *)this;
}

IAI *SceneObject::GetAI() const
{
    return AiStaff;
}

void SceneObject::SetAI(IAI *ai)
{
	if (AiStaff)
		delete AiStaff;
    AiStaff = (AIModule*)ai;
	AiStaff->SetOwner(this);
}

ICollidable *SceneObject::GetCollidable() const
{
	return CollidableModule.get();
}

IEquipped *SceneObject::GetEquipped() const
{
	return EquippedStaff.get();
}

IDestructable *SceneObject::GetDestructable() const
{
	return DestructableModule.get();
}

//IDescriptable *SceneObject::GetDescriptable() const
//{
//	return DescriptableReference;
//}

int SceneObject::GetDescriptableID() const
{
	return DescriptableID;
}

void SceneObject::SaveTo(TiXmlElement * root) const
{	
	if (LID>0)
	{
		root->SetAttribute("lid", LID);
	}
	AAObject::SaveTo(root);
	

	if (Type == PT_DYNAMIC)
		root->SetAttribute("physical", "dynamic");
	else
	if (Type == PT_NOT_PHYSYCS_DYNAMIC)
		root->SetAttribute("physical", "notphys_dynamic");
	else
		root->SetAttribute("physical", "static");

	if (RoomableModule.get())
		root->SetAttribute("roomable", "on");

	//if (InitialRotation!=Ogre::Quaternion::IDENTITY)
 //   {	
	//	Ogre::Degree angle;
	//	Ogre::Vector3 rotation_axis;
	//	rotation_axis = Ogre::Vector3::UNIT_X;
	//	InitialRotation.ToAngleAxis(angle, rotation_axis);
 //       root->SetAttribute("irx",angle.valueDegrees());
	//	
	//	rotation_axis = Ogre::Vector3::UNIT_Y;
	//	InitialRotation.ToAngleAxis(angle, rotation_axis);
 //       root->SetAttribute("iry",angle.valueDegrees());
	//	
	//	rotation_axis = Ogre::Vector3::UNIT_Z;
	//	InitialRotation.ToAngleAxis(angle, rotation_axis);
 //       root->SetAttribute("irz",angle.valueDegrees());
 //   }
	//	
	if (Irx!=0)
	{
		 root->SetAttribute("irx",Irx);
	}
	if (Iry!=0)
	{
		root->SetAttribute("iry",Iry);
	}
	if (Irz!=0)
	{
		root->SetAttribute("irz",Irz);
	}

	root->SetAttribute("modelname", Modelname);

	ICollidable *collid = GetCollidable();
	if (collid)
		collid->SaveTo(root);

	IPhysical *phys = GetPhysical();
	if (phys)
		phys->SaveTo(root);

	IDestructable *destr = GetDestructable();
	if (destr)
		destr->SaveTo(root);

	IEquipped *eq = GetEquipped();
	if (eq)	
		eq->SaveTo(root);
}

int SceneObject::GetTag() const
{
	return Tag;
}

void SceneObject::SetTag(int tag)
{
    Tag = tag;
}

//MemAllocator *SceneObject::GetAllocator()
//{
//	return &SceneObjectAllocator;
//}

//void SceneObject::Destroy()
//{
//	Poolable::Destroy(this, GetAllocator());
//}

Ogre::Vector3 SceneObject::GetPrevPosition() const
{
    return PrevPosition;
}

bool SceneObject::AddEffect(int lid, bool need_init)
{
	IMasterEffect *effect = ObjectsLibrary::GetInstance()->LoadEffect(lid);
				
            assert(effect);
                        
            if (effect) 
            {   
				if (need_init)
				{
					effect->Init();
					effect->SetParent(this);
					//effect->InitSounds();
				}
				if (NULL==Effects)
					Effects = new EffectsPool;
                Effects->PushBack(effect);
            }		
	
	return true;
}

bool SceneObject::RemoveEffect(int lid)
{
	IMasterEffect *effect;
	for (EffectsPool::ListNode *pos=Effects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		effect = pos->Value;
		if (effect->GetLID()==lid)
		{
			Effects->Remove(effect);
			effect->Destroy();
		}
	}
	return true;
}

bool SceneObject::RemoveEffect(const char* name)
{
	assert(false);
	IMasterEffect *effect;
	for (EffectsPool::ListNode *pos=Effects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		effect = pos->Value;
		if (strcmp(effect->GetName(),name)==0)
		{
			Effects->Remove(effect);
			effect->Destroy();
		}
	}
	return true;
}

void SceneObject::SetInitialDirection(Ogre::Vector3 dir)
{
	InitialDirection = dir;
}

BonusesKeeper *SceneObject::GetBonusesKeeper()
{
	return BonusesModule.get();
}

const char *SceneObject::GetModelName() const
{
	return Modelname;
}

ICombatable *SceneObject::GetCombatable() const
{
	return CombatableUnit.get();
}


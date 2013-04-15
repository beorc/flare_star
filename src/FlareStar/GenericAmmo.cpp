#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\genericammo.h"
#include "debugging.h"
#include "CommonDeclarations.h"
#include "Room.h"
#include "Roomable.h"
#include "RoomsManager.h"
#include "IAAObject.h"
#include "Collidable.h"
#include "IPhysical.h"

GenericAmmo::GenericAmmo() :
Ammo(),
SceneObject(),
Velocity(0)
{
    Tag = T_GENERICAMMO;	
}

GenericAmmo::~GenericAmmo(void)
{
}

void GenericAmmo::Init(Vector3 pos, Quaternion direction)
{	
	Ammo::Init(pos, direction);

	SetInitialPosition(pos);

	CommonDeclarations::AddObjectRequest(this);
}

bool GenericAmmo::Init()
{
	bool res = SceneObject::Init();

	Node->setOrientation(InitialOrientation);

	if (Modelname)
		Size = GetBoundingBox(false).getSize().z/2;
	
	return res;
}

bool GenericAmmo::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	
	/*str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}*/

	str = xml->Value();
	assert(strcmp(str,"ammo")==0);    
	
	Ammo::Parse(xml);
	SceneObject::Parse(xml);

	int res, Val;
	res = xml->QueryIntAttribute("velocity",&Velocity);
	Velocity = -Velocity;

	res = xml->QueryIntAttribute("size",&Val);
	assert(TIXML_SUCCESS==res);
	assert(Val>0);
	Size=(unsigned)Val;

	return true;
}

void GenericAmmo::Hit(IAAObject *obj)
{
	Ammo::Hit(obj);
	if (DestroyOnHit)
		CommonDeclarations::DeleteObjectRequest(this);
}

void GenericAmmo::Register()
{
	SceneObject::Init();
	SceneObject::Register();

	Orientation = InitialOrientation*Vector3::UNIT_Z;		
}

void GenericAmmo::Step(unsigned timeMs)
{
	SceneObject::Step(timeMs);

	//LastTime+=timeMs;
	//if (LastTime<OUTER_UPDATE_INTERVAL_MS)
	//	return;    
	assert(Node);
	if (NULL==Node)
		return;
	
	//char log[100];
	//sprintf(log,"ammo %d %d\n",LastTime, timeMs);
	//Debugging::Log("effects",log);
	Ogre::Vector3 origin = Node->getPosition()+Orientation*timeMs*Velocity;
	Node->setPosition(origin);

	CheckCollisions(origin, (IRoomable*)RoomableModule.get());
    	
    /*if (RoomableModule.get())
    {
        IRoomable::RoomsPool *pool = ((IRoomable*)RoomableModule.get())->GetRooms();
        if (NULL!=pool && !pool->IsEmpty())
        {   
            Room::ObjectType *object=NULL;
            
            Room::ObjectsPool *objects_pool = NULL;
            ICollidable *collid = NULL;
            
            objects_pool = pool->GetBegin()->Value->GetChildObjects();
            
			for (Room::ObjectsPool::ListNode *pos = objects_pool->GetBegin();pos!=NULL;pos=pos->Next)
            {
                object = pos->Value;

				if (object == this)
					continue;

				if (CheckHit(object, origin))
					return;
            }            	
        }
    }*/
}

void GenericAmmo::ApplyWeapon(const Weapon::WeaponDescription &descr)
{
	Ammo::ApplyWeapon(descr);
	Velocity += descr.SpeedBuff;
}

Ogre::AxisAlignedBox GenericAmmo::GetBoundingBox(bool transformed) const
{	
	if (!transformed)
	{
		Ogre::AxisAlignedBox box(-Size,-Size,-Size,Size,Size,Size);
		return box;
	} else
	{
		Ogre::Vector3 origin = Node->getPosition();
		Ogre::AxisAlignedBox box(origin.x-Size,origin.y-Size,origin.z-Size,origin.x+Size,origin.y+Size,origin.z+Size);
		return box;
	}	
}

//void GenericAmmo::Destroy()
//{
//	Poolable::Destroy(this);
//}

int GenericAmmo::GetLID() const
{
	return AAObject::GetLID();
}

void GenericAmmo::SetLID(int lid)
{
	AAObject::SetLID(lid);
}

int GenericAmmo::GetVelocity() const
{
	return Velocity;
}

bool GenericAmmo::IsMe(IAAObject *object) const
{
	if (this==object)
		return true;

	return false;
}

void GenericAmmo::UpdateBox()
{
}
#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "BillboardAmmo.h"
#include "debugging.h"
#include "CommonDeclarations.h"
#include "Room.h"
#include "Roomable.h"
#include "RoomsManager.h"
#include "IAAObject.h"
#include "Collidable.h"
#include "IPhysical.h"
#include <tinyxml.h>

#include "Roomable.h"

BillboardAmmo::BillboardAmmo() :
AAObject(),
BillboardSubEffect(),
Ammo(),
Velocity(0)
{
    Tag = T_BILLBOARDAMMO;	
}

BillboardAmmo::~BillboardAmmo(void)
{
	if (RoomableModule.get())
	{
		RoomableModule->RemoveFromRooms();
		RoomableModule->GetRooms()->Clear();	
	}
}

void BillboardAmmo::Init(Vector3 pos, Quaternion direction)
{	
	Ammo::Init(pos, direction);

	Position = pos;

	CommonDeclarations::AddObjectRequest(this);
}

bool BillboardAmmo::Init()
{
	//bool res = SceneObject::Init();
	
	bool res = BillboardSubEffect::Init();
			
	return res;
}

bool BillboardAmmo::Parse(TiXmlElement* xml)
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

	AAObject::Parse(xml);
	BillboardSubEffect::Parse(xml);
	Ammo::Parse(xml);

	RoomableModule.reset(new Roomable(this));
	RoomableModule->Parse(xml);

	int res;
	res = xml->QueryIntAttribute("velocity",&Velocity);
	Velocity = -Velocity;

	return true;
}

void BillboardAmmo::Hit(IAAObject *obj)
{
	Ammo::Hit(obj);
	if (DestroyOnHit)
		CommonDeclarations::DeleteObjectRequest(this);
}

void BillboardAmmo::Register()
{
	Init();
	AAObject::Register();
	
	Orientation = InitialOrientation*Vector3::UNIT_Z;		
}

void BillboardAmmo::Step(unsigned timeMs)
{
	//assert(OgreBillboard);
	//if (NULL==OgreBillboard)
	//	return;
	AAObject::Step(timeMs);
	//char log[100];
	//sprintf(log,"ammo %d %d\n",LastTime, timeMs);
	//Debugging::Log("effects",log);
	Position = Position+Orientation*timeMs*Velocity;	
	OgreBillboard->setPosition(Position);

	if (RoomableModule.get())
	{
		RoomableModule->SetBox(GetBoundingBox(true));
		CommonDeclarations::GetRoomManager()->UpdateObject(this);
		CheckCollisions(Position, (IRoomable*)RoomableModule.get());
	}
}

void BillboardAmmo::ApplyWeapon(const Weapon::WeaponDescription &descr)
{
	Ammo::ApplyWeapon(descr);
	Velocity += descr.SpeedBuff;
}

Ogre::AxisAlignedBox BillboardAmmo::GetBoundingBox(bool transformed) const
{	
	int size = BillboardSubEffect::Size;
	if (!transformed)
	{
		Ogre::AxisAlignedBox box(-size,-size,-size,size,size,size);
		return box;
	} else
	{		
		Ogre::AxisAlignedBox box(Position.x-size,Position.y-size,Position.z-size,Position.x+size,Position.y+size,Position.z+size);
		return box;
	}	
}

IRoomable *BillboardAmmo::GetRoomable() const
{
	return RoomableModule.get();
}

Ogre::Vector3 BillboardAmmo::GetPosition() const
{
	return Position;
}

int BillboardAmmo::GetLID() const
{
	return AAObject::GetLID();
}

void BillboardAmmo::SetLID(int lid)
{
	AAObject::SetLID(lid);
}

int BillboardAmmo::GetVelocity() const
{
	return Velocity;
}

bool BillboardAmmo::IsMe(IAAObject *object) const
{
	if (this==object)
		return true;

	return false;
}



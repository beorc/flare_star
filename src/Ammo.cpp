#include "StdAfx.h"
#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\ammo.h"
#include "Utilities.h"
#include "Room.h"
#include "RoomsManager.h"


Ammo::Ammo():
InitialOrientation(Ogre::Quaternion::IDENTITY),
Damping(0.f),
DamageRadius(0),
Damage(0), 
Kick(0),
DestroyOnHit(true),
AmmoGroup(IAAObject::FA_NONE),
Size(100)
{
}

Ammo::~Ammo(void)
{
}

void Ammo::Init(Vector3 pos, Quaternion direction)
{	
	InitialOrientation = direction;
}

bool Ammo::Parse(TiXmlElement* xml)
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

	int res, temp;
	res = xml->QueryFloatAttribute("damping",&Damping);
	res = xml->QueryIntAttribute("damage_radius",&temp);
	if (TIXML_SUCCESS==res)
		DamageRadius = (unsigned)temp;
	res = xml->QueryIntAttribute("damage",&Damage);
	res = xml->QueryIntAttribute("kick",&Kick);
	res = xml->QueryIntAttribute("destroy_on_hit",&temp);
	if (TIXML_SUCCESS==res)
		DestroyOnHit = temp ? DestroyOnHit = true : DestroyOnHit = false;
	
	res = xml->QueryIntAttribute("size",&Size);

	//str = xml->Attribute("ammo_group");
	//if (str)
	//{
	//	if (strcmp(str,"friend")==0)
	//		AmmoGroup = IAAObject::FA_FRIEND;
	//	else
	//		if (strcmp(str,"alien")==0)
	//			AmmoGroup = IAAObject::FA_ALIEN;
	//}

	return true;
}

void Ammo::ApplyWeapon(const Weapon::WeaponDescription &descr)
{	
	Kick += descr.KickBuff;
	Damage += descr.DamageBuff;	
	//InitialOrientation  Dispersion;
	
	if (descr.Dispersion!=0)
	{
		Ogre::Vector3 rot_axe(Ogre::Vector3::ZERO);
		rot_axe.x = AAUtilities::Rand() - AAUtilities::Rand();
		rot_axe.y = AAUtilities::Rand() - AAUtilities::Rand();
		//rot_axe.normalise();

		//rot_axe = Ogre::Vector3::UNIT_Y;
				
		Ogre::Quaternion q;
        Ogre::Radian angle(descr.Dispersion*AAUtilities::Rand());
        q.FromAngleAxis(angle, rot_axe);
        InitialOrientation = InitialOrientation*q;
	}	
}

void Ammo::Hit(IAAObject *obj)
{
	if (AmmoGroup!=IAAObject::FA_NONE && AmmoGroup==obj->GetGroup())
		return;
	IDestructable *destr = obj->GetDestructable();
	if (destr)
		destr->Damage(Damage);
}

void Ammo::SetAmmoGroup(int group)
{
	AmmoGroup = (IAAObject::EGroup)group;
}

bool Ammo::CheckHit(IAAObject *object, Ogre::Vector3 &origin)
{	
	bool havehit=false;
	ICollidable *collid = object->GetCollidable();

	if (collid)
	{	
		/*bool in_rad=false;
		IPhysical *phys = object->GetPhysical();
		assert(phys);

		Ogre::Vector3 point = object->GetPosition();
		Ogre::Vector3 dist = point-origin;
		
		double radius = phys->GetRadius();
		if (dist.squaredLength()<radius*radius)
			in_rad = true;*/

		Ogre::AxisAlignedBox box;

		box.setExtents(origin.x-Size,origin.y-Size,origin.z-Size,origin.x+Size,origin.y+Size,origin.z+Size);
		if (object->GetBoundingBox(true).intersects(box)) 		
		{
			if (collid->Collide(origin, 2*Size))
				havehit = true;

			Ogre::Vector3 opos = object->GetPosition(), dist = opos-origin;
			if (dist.squaredLength()<(2*Size)*(2*Size))
			{
				havehit = true;
			}

			if (havehit)
			{
				Hit(object);				
			}
		}
	}
	return havehit;
}

void Ammo::CheckCollisions(Ogre::Vector3 origin, IRoomable *rmbl)
{
	assert(rmbl);
	Room::ObjectType *object=NULL;
	if (rmbl)
	{
		IRoomable::RoomsPool *pool = rmbl->GetRooms();
		if (NULL!=pool && !pool->IsEmpty())
		{   
			Room::ObjectsPool *objects_pool = NULL;
			ICollidable *collid = NULL;

			objects_pool = pool->GetBegin()->Value->GetChildObjects();

			for (Room::ObjectsPool::ListNode *pos = objects_pool->GetBegin();pos!=NULL;pos=pos->Next)
			{
				object = pos->Value;

				if (IsMe(object))
					continue;
				
				if (CheckHit(object, origin))
					return;		
			}
		}
	}

	RoomsManager *rooms_manager = CommonDeclarations::GetRoomManager();
	if (rooms_manager->GetOuterObjectsNumber()>0)
	{
		RoomsManager::OuterObjectsContainer *outer_objects = rooms_manager->GetOuterObjects();

		size_t i=0;
		for (size_t i=0; i<outer_objects->Size; ++i)
		{
			object = outer_objects->Data[i];
			if (NULL==object)
				break;
			if (IsMe(object))
				continue;
			if (CheckHit(object, origin))
				return;
		}
	}
}

bool Ammo::IsMe(IAAObject *object) const
{
	return false;
}




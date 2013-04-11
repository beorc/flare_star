#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\eulerphysicsammo.h"

#include "debugging.h"
#include "CommonDeclarations.h"
#include "Room.h"
#include "Roomable.h"
#include "RoomsManager.h"
#include "IAAObject.h"
#include "Collidable.h"
#include "IPhysical.h"

EulerPhysicsAmmo::EulerPhysicsAmmo():
GenericAmmo()
{
	Type = PT_DYNAMIC;
	Tag = T_EULERAMMO;
}

EulerPhysicsAmmo::~EulerPhysicsAmmo(void)
{
}

void EulerPhysicsAmmo::Register()
{
	SceneObject::Register();
	
	Node->setOrientation(InitialOrientation);
	Ogre::Vector3 orientation = InitialOrientation*Ogre::Vector3::UNIT_Z;

	GetPhysical()->SetForwardDirection(orientation);	
	GetPhysical()->SetAcceleration(Ogre::Vector3(0,0,-1));
}

void EulerPhysicsAmmo::ApplyWeapon(const Weapon::WeaponDescription &descr)
{
	Ammo::ApplyWeapon(descr);
	//MaxVelocity += descr.SpeedBuff;
    Ogre::Vector3 speed_buff(descr.SpeedBuff,descr.SpeedBuff,descr.SpeedBuff);
	GetPhysical()->SetMaxVelocities(GetPhysical()->GetMaxVelocities()+speed_buff);
}

void EulerPhysicsAmmo::Step(unsigned timeMs)
{
	SceneObject::Step(timeMs);

	assert(Node);
	if (NULL==Node)
		return;

	Ogre::Vector3 origin = GetPhysical()->GetPosition();

	CheckCollisions(origin, (IRoomable*)RoomableModule.get());
	
	/*	
	Room::ObjectType *object=NULL;

	if (RoomableModule.get())
	{
		IRoomable::RoomsPool *pool = ((IRoomable*)RoomableModule.get())->GetRooms();
		if (NULL!=pool && !pool->IsEmpty())
		{   
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
	}
	RoomsManager *rooms_manager = CommonDeclarations::GetRoomManager();
	if (rooms_manager->GetOuterObjectsNumber()>0)
	{
		RoomsManager::OuterObjectsContainer *outer_objects = rooms_manager->GetOuterObjects();

		size_t i=0;
		for (size_t i=0; i<outer_objects->Size; ++i)
		{
			object = outer_objects->Data[i];
			if (CheckHit(object, origin))
				return;
		}
	}	*/
}

int EulerPhysicsAmmo::GetVelocity() const
{
	return GetPhysical()->GetMaxVelocities().z;
}

Ogre::AxisAlignedBox EulerPhysicsAmmo::GetBoundingBox(bool transformed) const
{	
	if (!transformed)
	{
		Ogre::AxisAlignedBox box(-Size,-Size,-Size,Size,Size,Size);
		return box;
	} else
	{
		Ogre::Vector3 origin = GetPhysical()->GetPosition();
		Ogre::AxisAlignedBox box(origin.x-Size,origin.y-Size,origin.z-Size,origin.x+Size,origin.y+Size,origin.z+Size);
		return box;
	}	
}








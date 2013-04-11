#pragma once
#include "Ammo.h"
#include "BillboardSubEffect.h"
#include "AAObject.h"

class Roomable;

class BillboardAmmo : public Ammo, public BillboardSubEffect, public AAObject
{
public:	
	OBJDESTROY_DEFINITION
	BillboardAmmo();
	virtual ~BillboardAmmo();

	virtual void Init(Vector3 pos, Quaternion direction);
	virtual bool Init();

	virtual bool Parse(TiXmlElement* xml);		
	virtual void Register();
	virtual void Hit(IAAObject *obj);
	virtual void Step(unsigned timeMs);
	virtual void ApplyWeapon(const Weapon::WeaponDescription &descr);

	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;
	
	//
	
	virtual IRoomable *GetRoomable() const;
	virtual Ogre::Vector3 GetPosition() const;

	virtual int GetLID() const;
	virtual void SetLID(int lid);

	virtual int GetVelocity() const;
	virtual bool IsMe(IAAObject *object) const;
	
protected:
	int Velocity;
	Ogre::Vector3 Orientation;	
	std::auto_ptr<Roomable> RoomableModule;	
	Ogre::Vector3 Position;	
};

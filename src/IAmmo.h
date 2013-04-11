#pragma once
#include <Ogre.h>
#include "Weapon.h"

class IAAObject;
class TiXmlElement;

class IAmmo
{
public:
	virtual void Init(Ogre::Vector3 pos, Ogre::Quaternion direction) = 0;
	virtual void Hit(IAAObject *obj) = 0;
	virtual bool Parse(TiXmlElement* xml) = 0;
	virtual void ApplyWeapon(const Weapon::WeaponDescription &descr) = 0;

	virtual int GetLID() const = 0;
	virtual void SetLID(int lid) = 0;

	virtual void Destroy() = 0;
	virtual void SetAmmoGroup(int) = 0;

	virtual int GetVelocity() const = 0;
	virtual bool IsMe(IAAObject *object) const = 0;
};



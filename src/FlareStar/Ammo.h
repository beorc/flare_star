#pragma once
#include <Ogre.h>

#include "XMLConfigurable.h"
#include "IAmmo.h"

class IAAObject;
class TiXmlElement;

class Ammo : public XMLConfigurable, public IAmmo
{
public:
	Ammo();
	virtual ~Ammo(void);

	virtual void Init(Ogre::Vector3 pos, Ogre::Quaternion direction)=0;
	virtual void Hit(IAAObject *obj);
	virtual bool Parse(TiXmlElement* xml);
	virtual void ApplyWeapon(const Weapon::WeaponDescription &descr);
	virtual void SetAmmoGroup(int);

	virtual void CheckCollisions(Ogre::Vector3 origin, IRoomable *rmbl);
	virtual bool CheckHit(IAAObject *object, Ogre::Vector3 &origin);
	virtual bool IsMe(IAAObject *object) const;
protected:
	Ogre::Quaternion InitialOrientation;
	float Damping;
	unsigned DamageRadius;
	int Damage, Kick;
	bool DestroyOnHit;
	IAAObject::EGroup AmmoGroup;
	int Size;
};



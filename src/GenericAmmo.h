#pragma once
#include "Ammo.h"
#include "SceneObject.h"

class GenericAmmo :	public Ammo, public SceneObject
{
public:	
	OBJDESTROY_DEFINITION
	GenericAmmo();
	virtual ~GenericAmmo();

	virtual void Init(Vector3 pos, Quaternion direction);
	virtual bool Init();

	virtual bool Parse(TiXmlElement* xml);		
	virtual void Register();
	virtual void Hit(IAAObject *obj);
	virtual void Step(unsigned timeMs);
	virtual void ApplyWeapon(const Weapon::WeaponDescription &descr);

	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;

	virtual int GetLID() const;
	virtual void SetLID(int lid);

	virtual int GetVelocity() const;
	virtual bool IsMe(IAAObject *object) const;

	virtual void UpdateBox();

protected:
	int Velocity;
	Ogre::Vector3 Orientation;	
};

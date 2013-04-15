#pragma once
#include "ammo.h"
#include "GenericAmmo.h"

class EulerPhysicsAmmo : public GenericAmmo
{
public:		
	EulerPhysicsAmmo();
	virtual ~EulerPhysicsAmmo(void);
	
	virtual void Register();
	virtual void Step(unsigned timeMs);
	virtual void ApplyWeapon(const Weapon::WeaponDescription &descr);

	virtual int GetVelocity() const;	
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;

protected:
};

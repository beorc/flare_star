#pragma once
#include "weapon.h"

class GenericWeapon : public Weapon
{
public:
	GenericWeapon();
	virtual ~GenericWeapon(void);

	void SaveTo(TiXmlElement * root) const;
	
protected:
	void Shoot();
};

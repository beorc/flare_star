#pragma once

#include <Ogre.h>
#include "ISerializable.h"
#include "List.h"

class Equipment;
class IAAObject;

class IEquipped : public ISerializable
{
public:    
	struct SBonus
	{
		int LID, DescriptableID;
		bool Inited;
		char DropProbability;
	};
	typedef List<SBonus> BonusesContainer;
    virtual bool AddEquipment(Equipment* equipment) = 0;
	virtual Ogre::Ray GetSightRay() = 0;
	virtual void SetShooting(bool state) = 0;
    virtual void SetTargetPosition(Ogre::Vector3 pos) = 0;
	virtual void SetTargetPosition(Ogre::Vector3 pos, int distance) = 0;
	virtual BonusesContainer *GetBonuses() = 0;
	virtual void Drop(bool prob_mode=true) = 0;
	virtual IAAObject *GetOwner() = 0;
    virtual void AddBonus(IEquipped::SBonus bonus) = 0;

	virtual Ogre::Vector3 CalculateTargetPosition(Ogre::Vector3 pos, Ogre::Vector3 velocity) = 0;
	virtual bool TryToShoot(Ogre::Vector3 tpos, int tradius) = 0;
	virtual Ogre::Vector3 GetSightOrigin() const = 0;
};

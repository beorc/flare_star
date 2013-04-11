#pragma once

#include "Equipment.h"
#include "XMLConfigurable.h"
#include "IEquipped.h"

#include <vector>

class SceneObject;

class EquippedObject : public XMLConfigurable, public IEquipped
{
public:
	struct SSlot {
		Ogre::Vector3 RelativePos;
		int		Type;		
		Equipment*	  ChildEquipment;
	};
	//typedef std::vector<Equipment*> EquipmentsPool;
	typedef std::vector<SSlot> SlotsPool;
		
	EquippedObject(void);
	EquippedObject(SceneObject *);
	virtual ~EquippedObject(void);

	virtual bool AddEquipment(Equipment* equipment);
//	virtual bool TestToAdd(int slot_type);	

	virtual void Step( unsigned timeMs );
	virtual bool Parse(TiXmlElement* xml);

	void SetOwner(SceneObject *owner);
	IAAObject *GetOwner();
	SlotsPool *GetSlots();
	Equipment *GetFirstWeapon();
	virtual void SetShooting(bool state);
	virtual void SetTargetPosition(Ogre::Vector3 pos);
	virtual void SetTargetPosition(Ogre::Vector3 pos, int distance);

	virtual Ogre::Vector3 CalculateTargetPosition(Ogre::Vector3 pos, Ogre::Vector3 velocity);
	bool TryToShoot(Ogre::Vector3 tpos, int tradius);
	void CalculateSightOrigin();
	Ogre::Vector3 GetSightOrigin() const;
	
	virtual Ogre::Ray GetSightRay();

	virtual void Init();

	virtual void SaveTo(TiXmlElement * root) const;
	
	virtual void Drop(bool prob_mode=true);
	virtual void Clear();
	IEquipped::BonusesContainer *GetBonuses();
    void AddBonus(IEquipped::SBonus bonus);
protected:
	bool CheckWeaponToShoot(Ogre::Vector3 tpos, int tradius);
	//EquipmentsPool Equipments;	
	SlotsPool		Slots;
    SceneObject     *Owner;
	bool Shooting;
	//Ogre::Vector3 Origin;
	IEquipped::BonusesContainer Bonuses;
	int AmmoVelocity;
	Ogre::Vector3 RelativeSightOrigin;
};

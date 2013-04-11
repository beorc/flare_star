#pragma once

#include "IPhysical.h"
#include "IRoomable.h"
#include "IScriptable.h"
#include "IScenable.h"
#include "ICollidable.h"
#include "IEquipped.h"
#include "IAI.h"
#include "IDestructable.h"
#include "ISerializable.h"
#include "IDescriptable.h"
#include "Combat/ICombatable.h"

//#include "MemoryPool.h"

class BonusesKeeper;

class IAAObject : public ISerializable
{
public:
	enum EGroup {FA_NONE, FA_FRIEND, FA_ALIEN};
    virtual IPhysical *GetPhysical() const = 0;
    virtual IRoomable *GetRoomable() const = 0;
    virtual IScriptable *GetScriptable() const = 0;
    virtual IScenable *GetScenable() const = 0;
    virtual IAI *GetAI() const = 0;
	virtual ICollidable *GetCollidable() const = 0;
	virtual IEquipped *GetEquipped() const = 0;
	virtual IDestructable *GetDestructable() const = 0;
	//virtual IDescriptable *GetDescriptable() const = 0;
	virtual int GetDescriptableID() const = 0;
	virtual ICombatable *GetCombatable() const = 0;

    virtual void SetAI(IAI *) = 0;
	
    virtual void Destroy() = 0;
	virtual void QueryDestroy() = 0;	

	virtual void Register() = 0;
	virtual void Unregister() = 0;
	virtual void Step(unsigned timeMs) = 0;
	virtual bool IsRemoving() = 0;
	virtual void SetRemoving(bool on) = 0;
	virtual bool GetRegistered() = 0;

	virtual int GetTag() const = 0;

	virtual Ogre::Vector3 GetPosition() const = 0;
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool) const = 0;
	virtual bool IsVolatile() const = 0;
	virtual void SetActive(bool on) = 0;
	virtual bool GetActive() const = 0;
	virtual int GetGroup() const = 0;
	virtual void SetGroup(int group) = 0;

	virtual int GetLID() const = 0;
	virtual void SetLID(int lid) = 0;
	virtual bool RestoreBackupPosition() = 0;

    virtual void SetPosition(Ogre::Vector3) = 0;

	virtual BonusesKeeper *GetBonusesKeeper() = 0;
	virtual bool IsEditable() const = 0;
	virtual void SetEditable(bool) = 0;
};



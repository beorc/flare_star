#pragma once

#include "XMLConfigurable.h"
#include "IAAObject.h"
#include "CommonDeclarations.h"

class AAObject : public XMLConfigurable, public IScriptable, public IAAObject
{
public:	
	OBJDESTROY_DEFINITION
	AAObject();
	virtual ~AAObject(void);

	virtual void Register();
	virtual void Unregister();
	virtual void Step(unsigned timeMs);

	bool IsRemoving();
	void SetRemoving(bool on=true);

	virtual int GetID();
	virtual void SetID(int id);

	virtual void SetScriptValue(int index, char val);
	virtual char GetScriptValue(int index);

	virtual void QueryDestroy();
	//virtual void Destroy();
    	
    virtual bool Parse(TiXmlElement* xml);

    virtual IPhysical *GetPhysical() const;
    virtual IRoomable *GetRoomable() const;
    virtual IScriptable *GetScriptable() const;
    virtual IScenable *GetScenable() const;
    virtual IAI *GetAI() const;
	virtual ICollidable *GetCollidable() const;
	virtual IEquipped *GetEquipped() const;
	virtual IDestructable *GetDestructable() const;
	//virtual IDescriptable *GetDescriptable() const;
	virtual int GetDescriptableID() const;
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool) const;
	virtual ICombatable *GetCombatable() const;

	virtual bool RestoreBackupPosition();

		
	bool GetRegistered();

	virtual void SaveTo(TiXmlElement * root) const;

	virtual int GetTag() const;
	virtual void SetTag(int tag);

	bool IsVolatile() const;
	virtual void SetActive(bool on);
	virtual bool GetActive() const;

	virtual bool Killed();
	int GetGroup() const;
	void SetGroup(int group);

	virtual int GetLID() const;
	virtual void SetLID(int lid);
    virtual void SetPosition(Ogre::Vector3);

    virtual void SetAI(IAI *);

	virtual BonusesKeeper *GetBonusesKeeper();

	virtual bool IsEditable() const;
	virtual void SetEditable(bool);
protected:
	bool Removing, Registered, Volatile;
    int ID;
	char ScriptValues[10];
	int  Tag;
	int LifeTime;
	bool LifeTimeMode;
	char *ScriptHandlers[3];
	bool Active;
	IAAObject::EGroup Group;
	int LID;
	bool Editable;
};

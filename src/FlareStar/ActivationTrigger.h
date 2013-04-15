#pragma once
#include "Trigger.h"
#include "Array.h"
#include "AAObject.h"

class Room;

class ActivationTrigger : public Trigger, public AAObject
{
public:
	OBJDESTROY_DEFINITION
	ActivationTrigger(void);
	virtual ~ActivationTrigger(void);

	virtual void Step( unsigned timeMs );
	virtual bool Parse(TiXmlElement* xml);
    virtual bool Init();

	virtual bool ApplyIn(IAAObject*);
	virtual bool ApplyOut(IAAObject*);

	virtual void Register();

	virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;
	virtual void SetActive(bool on);

	bool RemoveFromActivationObjects(IAAObject*);
	const Array<IAAObject*> *GetActivationObjects() const;
protected:
	Ogre::AxisAlignedBox ActivationBox;
	
	Array<Room*> ActivationRooms;
	Array<IAAObject*> ActivationObjects;
};

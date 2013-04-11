#pragma once
#include "List.h"
#include "IAAObject.h"
#include "XMLConfigurable.h"

class Trigger : public XMLConfigurable
{
public:
	struct STriggeredObject
	{
		STriggeredObject() :
		Object(NULL),
		Triggering(false)
		{
		}
		IAAObject *Object;
		bool Triggering;
	};
	typedef List<STriggeredObject> ObjectsContainer;
	Trigger(void);
	virtual ~Trigger(void);

	virtual void Step( unsigned timeMs );
	virtual bool Parse(TiXmlElement* xml);
    virtual bool Init();

	virtual bool ApplyIn(IAAObject*);
	virtual bool ApplyOut(IAAObject*);
    virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;
	virtual void SetTriggersPosition(Ogre::Vector3);	

	virtual void AddControllingObject(IAAObject *obj);
protected:	 
	 //unsigned LastTime;
     int Size;
	 Ogre::Vector3 TriggersPosition;
	 ObjectsContainer ControllingObjects;
	 Ogre::AxisAlignedBox TriggersBox;
	 unsigned BufferZoneSize;
	 
	 char *TriggersScriptHandlers[2];
};

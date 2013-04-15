#pragma once

#include "Debugging.h"
//#include "XMLConfigurable.h"

#include <Ogre.h>
using namespace Ogre;


#include "SceneObject.h"
//class SceneObject;

class Equipment : public SceneObject
{
public:
	enum EQUIPMENT_TYPE {ET_NONE, ET_WEAPON};
	Equipment(EQUIPMENT_TYPE type=ET_NONE);
	virtual ~Equipment(void);

	Vector3 GetRelativePos();
	void SetRelativePos(Vector3 pos);

	virtual void SetParent(IScenable *parent);

	EQUIPMENT_TYPE GetType();
	int GetSlotType();

	void SetModel(const char* modelname);
	virtual bool Parse(TiXmlElement* xml);
	virtual void SetOwner(IEquipped *);

	virtual bool Init();
	virtual void UpdateBox();
protected:	
	Ogre::Vector3 RelativePos;

	IScenable *Parent;
	IEquipped *Owner;
	EQUIPMENT_TYPE Type;
	bool Inited;
	
	int SlotType;
};

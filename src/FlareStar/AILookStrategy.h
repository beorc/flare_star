#pragma once
#include "AIStrategy.h"
#include <Ogre.h>

#include "InterpolatedRotation.h"

class IScenable;

class AILookStrategy : public AIStrategy
{
public:
	enum ELookType {LT_NONE,LT_OBJECT,LT_DIRECTION,LT_FORWARD};
	AILookStrategy(void);
	~AILookStrategy(void);

	virtual void Step(unsigned timeMs);	
	void SetDirection(Ogre::Vector3 dir);
	virtual bool Parse(TiXmlElement* xml);
	virtual void Init();
protected:	
	//IScenable* Target;
	Ogre::Vector3 Direction;

	InterpolatedRotation RotationUnit;
	//unsigned prevtime;
	ELookType LookType;
	//int TargetID;
};

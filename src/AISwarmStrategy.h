#pragma once
#include "aistrategy.h"
#include <Ogre.h>

#include "InterpolatedRotation.h"

class IScenable;

class AISwarmStrategy : public AIStrategy
{
public:
	AISwarmStrategy(void);	
	~AISwarmStrategy(void);

	virtual void Step(unsigned timeMs);
	void SetTargetPosition(Ogre::Vector3 pos);		
	virtual bool Parse(TiXmlElement* xml);
protected:
    
	InterpolatedRotation RotationUnit;
	Ogre::Vector3 TargetPosition, PrevOrientDirection;
};

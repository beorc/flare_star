#pragma once
#include "aistrategy.h"
#include <Ogre.h>

#include "InterpolatedRotation.h"

class IScenable;

class AIInConeAttackStrategy : public AIStrategy
{
public:
	AIInConeAttackStrategy(void);	
	~AIInConeAttackStrategy(void);

	virtual void Step(unsigned timeMs);
	virtual bool Parse(TiXmlElement* xml);
    //void SetAngleCosinusModule(Ogre::Real);
protected:	

	InterpolatedRotation RotationUnit;
	int RotationSpeed;
    Ogre::Real AngleCosinusModule;
};

#pragma once
#include "aistrategy.h"
#include <Ogre.h>

#include "InterpolatedRotation.h"

class IScenable;

class AISightNAttackStrategy : public AIStrategy
{
public:
	AISightNAttackStrategy(void);	
	~AISightNAttackStrategy(void);

	virtual void Step(unsigned timeMs);
	//void SetTargetObject(IScenable* obj);		
	virtual bool Parse(TiXmlElement* xml);
protected:	
	//IScenable* Target;

	InterpolatedRotation RotationUnit;
	unsigned prevtime;
	int RotationSpeed;
};

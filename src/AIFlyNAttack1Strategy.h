#pragma once
#include "aistrategy.h"
#include <Ogre.h>
#include "AISwarmStrategy.h"

#include "InterpolatedRotation.h"

class IScenable;

class AIFlyNAttack1Strategy : public AIStrategy
{
public:
	AIFlyNAttack1Strategy(void);	
	~AIFlyNAttack1Strategy(void);

	virtual void Step(unsigned timeMs);
	//void SetTargetObject(IScenable* obj);		
	virtual bool Parse(TiXmlElement* xml);
	virtual void Init();
protected:
    enum EFlyNAttack1State {FA1_NONE, FA1_SEEKING, FA1_FLEEING, FA1_SEEKING2FLEEING, FA1_FLEEING2SEEKING, FA1_BREAKAWAY};
    EFlyNAttack1State FlyNAttack1State, PrevState;
	//IScenable* Target;

	InterpolatedRotation RotationUnit;
	unsigned prevtime;
	int RotationSpeed;
    int MinDistance, MaxDistance, BufferZone;
    Ogre::Vector3 FleeingDirection, PrevOrientDirection;
    int MaxFleeingTime, FleeingTime;
	AISwarmStrategy SwarmStrategy;
};

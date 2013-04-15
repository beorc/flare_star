#pragma once
#include "aistrategy.h"
#include <Ogre.h>

#include "InterpolatedRotation.h"

class AIAutoPilotStrategy : public AIStrategy
{
public:
	enum ETargetType {TT_NONE,TT_OBJECT,TT_POINT,TT_INACTIVE};

	AIAutoPilotStrategy(void);
	~AIAutoPilotStrategy(void);

	virtual void Step(unsigned timeMs);	
	virtual bool Parse(TiXmlElement* xml);

	void SetTargetPoint(Ogre::Vector3 point);
	Ogre::Vector3 GetTargetPoint() const;

	virtual void SetTargetID(int id);

	virtual void RemoveTarget();
	virtual ETargetType GetTargetType() const;
	void SetTargetType(ETargetType);

	void SetActive(bool on);
	bool GetActive() const;
protected:    
	InterpolatedRotation RotationUnit;	
	Ogre::Vector3 TargetPoint;
	ETargetType TargetType;
	bool Active;
};

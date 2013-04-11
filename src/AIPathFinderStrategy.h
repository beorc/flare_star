#pragma once
#include "aistrategy.h"

class IScenable;

class AIPathFinderStrategy : public AIStrategy
{
public:
	enum ETargetType {TT_NONE,TT_OBJECT,TT_POINT,TT_WAYPOINT};
	enum EFlyStatesType {FST_NONE,FST_POINTREACHED,FST_MOVING};
    
	AIPathFinderStrategy(void);
	~AIPathFinderStrategy(void);

	void SetTargetPoint(Ogre::Vector3 point);
	Ogre::Vector3 GetTargetPoint();
	/*void SetTargetObject(IScenable *object);
	IScenable *GetTargetObject();*/

	EFlyStatesType GetFlyState();

	virtual bool Parse(TiXmlElement* xml);
	virtual void Init();
    virtual void Step(unsigned timeMs);

	virtual void SetTargetID(int id);
    virtual bool IsTargetReached(Ogre::Vector3 target);

	virtual void RemoveTarget();
	virtual ETargetType GetTargetType() const;
protected:
	ETargetType TargetType;
	EFlyStatesType State;

	Ogre::Vector3 TargetPoint;
	//IScenable *TargetObject;
	int DistanceToTarget;
    int WaitTargetTimeout;
};

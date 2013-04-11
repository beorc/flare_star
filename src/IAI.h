#pragma once

class AIModule;
class AICommander;
class AIStrategy;
class AIPathFinderStrategy;
class AIAutoPilotStrategy;

class IAI
{
public:   
    enum EUnitType {UT_NONE,UT_MASTER,UT_SLAVE};
	enum EAttackMode {AM_NONE, AM_ALIENS, AM_FRIENDS};

    virtual AICommander *GetCommander() = 0;
	virtual void SetLead(AICommander *lead) = 0;
    virtual AIPathFinderStrategy *GetPathFinder() = 0;
    virtual void SetUnitType(EUnitType type) = 0;
    virtual EUnitType GetUnitType() = 0;
    virtual void AddStrategy(AIStrategy *strategy) = 0;
	virtual int SelectTargetID() = 0;
	virtual AIAutoPilotStrategy *GetAutoPilotStrategy() const = 0;
	virtual bool GetActive() const = 0;
	virtual void SetActive(bool on) = 0;
};

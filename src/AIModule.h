#pragma once
#include "XMLConfigurable.h"
#include "IAAObject.h"
#include "IAI.h"
#include "List.h"

class AICommander;
class AIPathFinderStrategy;
class AIStrategy;
class AIAutoPilotStrategy;

class AIModule : public IAI
{
public:    
    //typedef std::vector<AIStrategy *> StartegiesPool;	
	typedef List<AIStrategy *> StartegiesPool;
    
    AIModule(void);
    //AIModule(IAAObject* owner);	
    virtual ~AIModule(void);

    IAAObject *GetOwner();
    void SetOwner(IAAObject *owner);

    void StepAI(unsigned timeMs);

    AICommander *GetCommander();
	virtual void SetLead(AICommander *lead);
    AIPathFinderStrategy *GetPathFinder();
	AIAutoPilotStrategy *GetAutoPilotStrategy() const;

    void SetUnitType(EUnitType type);
    EUnitType GetUnitType();

    void AddStrategy(AIStrategy *strategy);

    virtual bool Parse(TiXmlElement* xml);	

	virtual bool	Init();
	virtual int SelectTargetID();

    bool TryToDestroy();

	virtual bool GetActive() const;
	virtual void SetActive(bool on);
private:
    void EraseStrategy(AIStrategy *strategy);

    EUnitType      UnitType;
	EAttackMode		AttackMode;

    //unsigned LastTime;


    AICommander *CommanderModule, *LeadModule;
    AIPathFinderStrategy *PathFinder;
	AIAutoPilotStrategy *AutoPilot;

    StartegiesPool Strategies;

    IAAObject *Owner;
	bool Active;
};

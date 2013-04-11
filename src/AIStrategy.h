#pragma once

#include <vector>
//#include <list>

#include "IAAObject.h"

#include <Ogre.h>
using namespace Ogre;

#include "XMLConfigurable.h"

class IAI;

class AIStrategy : public XMLConfigurable
{
public:	
	enum STRATEGY_TYPE {ST_UNKNOWN, ST_PATHFINDER, ST_COMMANDER, ST_AUTOPILOT};
	AIStrategy(STRATEGY_TYPE type=ST_UNKNOWN);
	virtual ~AIStrategy(void);

	virtual void Step(unsigned timeMs) = 0;
	virtual void SetParent(IAAObject* parent);
    virtual IAAObject* GetParent();
	STRATEGY_TYPE GetType() const;
	virtual void Init();	
	
	virtual void SetTargetID(int id);
	virtual int GetTargetID() const;
	void SetOwner(IAI* owner);

	virtual bool Parse(TiXmlElement* xml);
protected:
	IAAObject* Parent;
	STRATEGY_TYPE Type;
	int TargetID;
	IAI* Owner;
};

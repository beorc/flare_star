#pragma once
#include "aicommander.h"

class TiXmlElement;

class AIAckerFormationCommander :	public AICommander
{
public:    
	AIAckerFormationCommander(void);
	virtual ~AIAckerFormationCommander(void);

	virtual void Coordinate();
    virtual bool Parse(TiXmlElement* xml);
protected:
	int ShiftSize;
    //enum ECoordinationMode {ACM_NONE, ACM_PATHFINDER, ACM_FORCEPOSITION};
    //ECoordinationMode CoordinationMode;
};

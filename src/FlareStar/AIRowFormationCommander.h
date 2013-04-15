#pragma once
#include "aicommander.h"

class TiXmlElement;

class AIRowFormationCommander :	public AICommander
{
public:    
	AIRowFormationCommander(void);
	virtual ~AIRowFormationCommander(void);

	virtual void Coordinate();
    //virtual bool Parse(TiXmlElement* xml);
protected:
    //enum ECoordinationMode {ACM_NONE, ACM_PATHFINDER, ACM_FORCEPOSITION};
    //ECoordinationMode CoordinationMode;
};

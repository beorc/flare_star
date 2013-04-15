#pragma once
#include "AIStrategy.h"
#include "IAAObject.h"

#include <vector>
#include "List.h"

#include <Ogre.h>
using namespace Ogre;

class AICommander : public AIStrategy
{
public:
	enum EFormationType {FT_NONE,FT_ROW};
	typedef std::vector<IAAObject*> SlavesPool;
	typedef List<Vector3> WaypointsPool;
	enum {MAX_WAYPOINTS_NUMBER = 10};
	AICommander(void);
	~AICommander(void);
	virtual void Coordinate() = 0;
	virtual void Step(unsigned timeMs);
	virtual void SetParent(IAAObject* parent);
	void AddSlave(IAAObject* unit);
	void RemoveSlave(IAAObject* unit);
	virtual bool Parse(TiXmlElement* xml);
	virtual bool PassToSlave();
	virtual void Init();
protected:
	SlavesPool Slaves;
	WaypointsPool Waypoints;
	
	Ogre::SceneNode *DebugNodes[MAX_WAYPOINTS_NUMBER];
	double ParentSize;
    int WaypointsDistance;
    int FixedWayPoints;
};

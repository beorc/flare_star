#pragma once

#include <vector>
using namespace std;

#include <Ogre.h>
using namespace Ogre;

struct Waypoint
{
public:
	Waypoint(void);
	~Waypoint(void);

	bool IsInVisibleWaypoints(unsigned id);

	int ID;
	//Vector3 Position;
	std::vector<Waypoint*> VisibleWaypoints;

	Ogre::SceneNode* Node;
};

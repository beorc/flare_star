#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\waypoint.h"

Waypoint::Waypoint(void) : Node(NULL)
{
}

Waypoint::~Waypoint(void)
{
}

bool Waypoint::IsInVisibleWaypoints(unsigned id)
{
	std::vector<Waypoint*>::iterator iPos = VisibleWaypoints.begin(), iEnd = VisibleWaypoints.end();

	for (;iPos!=iEnd;++iPos)
	{
		if ((*iPos)->ID==id)
			return true;
	}
	return false;
}

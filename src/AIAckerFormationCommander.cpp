#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AiAckerFormationCommander.h"
#include "debugging.h"
//#include "Core.h"
//#include "CommonDeclarations.h"
#include "AIPathFinderStrategy.h"
#include "AIModule.h"
#include "IScenable.h"
#include "IAI.h"

AIAckerFormationCommander::AIAckerFormationCommander(void) : 
AICommander(),
ShiftSize(2000)
{
}

AIAckerFormationCommander::~AIAckerFormationCommander(void)
{
}

void AIAckerFormationCommander::Coordinate()
{	
	//if (!descriptor->Collided)
	//	Parent->GetNode()->setOrientation(descriptor->ForwardDirection.getHorizontalAngle());
    IScenable *scen = Parent->GetScenable();
    assert(scen);

	Vector3 OwnPos = scen->GetPosition();
	SlavesPool::iterator iSlavesPos = Slaves.begin(), iSlavesEnd = Slaves.end();
	//WaypointsPool::iterator iWaypointsPos = Waypoints.begin(), iWaypointsEnd = Waypoints.end();
	
	/*
	if (NULL==path_finder)
	{
		char log[100];
		sprintf(log,"Row formation: path finder not found (%d)\n",(*iSlavesPos)->GetID());
		Debugging::Log("Warnings.log",log);
		return;
	}
	*/	
	//char log[100];
	int wp_number=1;
	for (WaypointsPool::ListNode *pos = Waypoints.GetBegin(); pos!=NULL; pos=pos->Next, ++wp_number)
	{
		for (int i=0;iSlavesPos!=iSlavesEnd && i<2;++iSlavesPos, ++i)
		{
			int shift_size= wp_number*ShiftSize;
			int shift = -1*shift_size+i*2*shift_size;

			Ogre::Quaternion quat = scen->GetOrientation();
			Ogre::Vector3 target=quat*Ogre::Vector3::UNIT_X;
			target = pos->Value + target*shift;			

			AIPathFinderStrategy *path_finder;
			path_finder = (*iSlavesPos)->GetAI()->GetPathFinder();
			Vector3 curr_target = path_finder->GetTargetPoint();
			AIPathFinderStrategy::EFlyStatesType state =  path_finder->GetFlyState();
			if (target!=curr_target/* && (state == AIPathFinderStrategy::FST_POINTREACHED || state == AIPathFinderStrategy::FST_NONE)*/)
			{				
				path_finder->SetTargetPoint(target);
			}
		}
		if (iSlavesPos==iSlavesEnd)
			break;
	}
}

bool AIAckerFormationCommander::Parse(TiXmlElement* xml)
{
    AICommander::Parse(xml);
    if (xml == 0)
        return false; // file could not be opened

    //const char *str;	

    // parse the file until end reached

    //str = xml->Value();

    int val=0, res;	
    res = xml->QueryIntAttribute("shift_size", &ShiftSize);
    //assert(TIXML_SUCCESS==res);

    return true;
}



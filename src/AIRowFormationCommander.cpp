#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\airowformationcommander.h"
#include "debugging.h"
//#include "Core.h"
//#include "CommonDeclarations.h"
#include "AIPathFinderStrategy.h"
#include "AIModule.h"
#include "IScenable.h"
#include "IAI.h"

AIRowFormationCommander::AIRowFormationCommander(void) : 
AICommander()/*,
CoordinationMode(ACM_NONE)*/
{
}

AIRowFormationCommander::~AIRowFormationCommander(void)
{
}

void AIRowFormationCommander::Coordinate()
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
	for (WaypointsPool::ListNode *pos = Waypoints.GetBegin();iSlavesPos!=iSlavesEnd && pos!=NULL;++iSlavesPos, pos=pos->Next)
	{
        Vector3 target=pos->Value;
		//if (ACM_PATHFINDER == CoordinationMode) 
        {
            AIPathFinderStrategy *path_finder;
            path_finder = (*iSlavesPos)->GetAI()->GetPathFinder();
            Vector3 curr_target = path_finder->GetTargetPoint();
            AIPathFinderStrategy::EFlyStatesType state =  path_finder->GetFlyState();
            if (target!=curr_target && (state == AIPathFinderStrategy::FST_POINTREACHED || state == AIPathFinderStrategy::FST_NONE))
            {
                /*sprintf(log,"Row formation 1: (%d) (%f, %f, %f)\n",(*iSlavesPos)->GetScriptable()->GetID() ,target.x, target.y, target.z);
                Debugging::Log("lc",log);*/
                path_finder->SetTargetPoint(target);
            }
		}/* else
            if (ACM_FORCEPOSITION == CoordinationMode)
            {
                (*iSlavesPos)->SetPosition(target);
            }*/
		
	}
}

//bool AIRowFormationCommander::Parse(TiXmlElement* xml)
//{
//    AICommander::Parse(xml);
//    if (xml == 0)
//        return false; // file could not be opened
//
//    //const char *str;	
//
//    // parse the file until end reached
//
//    //str = xml->Value();
//
//    int val=0, res;	
//    res = xml->QueryIntAttribute("coord_mode", &val);
//    assert(TIXML_SUCCESS==res);
//
//    if (TIXML_SUCCESS==res)
//        CoordinationMode = (ECoordinationMode)val;
//
//    return true;
//}



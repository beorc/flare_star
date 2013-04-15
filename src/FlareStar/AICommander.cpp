#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\aicommander.h"
//#include "Core.h"
#include "CommonDeclarations.h"
#include "SceneObject.h"
#include "AIModule.h"
#include "IAI.h"
#include "AIPathFinderStrategy.h"


//#define DEBUG_WAYPOINTS_SEQUENCE

//static const int WaypointsDistance = 1000;
//static const int WaypointsDistanceSQ = WaypointsDistance*WaypointsDistance;

AICommander::AICommander(void) : 
AIStrategy(ST_COMMANDER),
Waypoints(MAX_WAYPOINTS_NUMBER),
ParentSize(0),
WaypointsDistance(3000),
FixedWayPoints(false)
{
}

AICommander::~AICommander(void)
{	
}

bool AICommander::PassToSlave()
{
	if (!Slaves.empty())
	{
		IAAObject *new_commander = Slaves[0];	
		
		/*AIPathFinderStrategy *path_finder = Parent->GetAI()->GetPathFinder();
		if (path_finder)
		{
			new_commander->GetAI()->GetPathFinder()->SetTargetID(path_finder->GetTargetID());
		}

		new_commander->GetAI()->AddStrategy(this);*/
		RemoveSlave(new_commander);
        new_commander->SetAI(Owner);
		
		return true;
	}
	return false;
}

void AICommander::Step(unsigned timeMs)
{		
    IScenable *scen = Parent->GetScenable();
	static Vector3 LastWaypointPos(scen->GetPosition());
	Vector3 OwnPos = scen->GetPosition();

    if (FixedWayPoints)
    {
        size_t i=1;
        for (WaypointsPool::ListNode *pos = Waypoints.GetBegin(); pos!=NULL; pos=pos->Next, ++i)
        {
            Ogre::Vector3 direction = scen->GetOrientation()*Ogre::Vector3::UNIT_Z;
            pos->Value = OwnPos + (WaypointsDistance+ParentSize)*direction*i;            
        }
        Coordinate();
    } else
    {
        if (!Waypoints.IsEmpty())
        {
            Vector3 shift;
            shift = Waypoints.GetBegin()->Value-OwnPos;
            //if (shift.squaredLength()<WaypointsDistanceSQ)
            //	return;
        }

        if ((OwnPos-LastWaypointPos).squaredLength()>(WaypointsDistance+ParentSize)*(WaypointsDistance+ParentSize))
        {		
            if (Waypoints.GetSize()>=MAX_WAYPOINTS_NUMBER)
                Waypoints.PopBack();
            Waypoints.PushFront(LastWaypointPos);		

            LastWaypointPos=OwnPos;

#ifdef DEBUG_WAYPOINTS_SEQUENCE
            WaypointsPool::iterator iPos = Waypoints.begin(), iEnd = Waypoints.end();
            for (int i=0;iPos!=iEnd;++iPos, ++i)
                DebugNodes[i]->setPosition(*iPos);
#endif
            Coordinate();
        }
    }
}

void AICommander::Init()
{
	AIStrategy::Init();
	if (Parent)
	{
		IPhysical *phys = Parent->GetPhysical();
		if (phys)
			ParentSize = phys->GetRadius();
	}
    if (FixedWayPoints)
    {
		Waypoints.Clear();
        for (size_t i=0; i<MAX_WAYPOINTS_NUMBER; ++i)
        {
            Waypoints.PushBack(Ogre::Vector3::ZERO);
        }
    }
}

void AICommander::SetParent(IAAObject* parent)
{
	AIStrategy::SetParent(parent);
	/*IPhysical *phys = parent->GetPhysical();
	if (phys)
		ParentSize = phys->GetRadius();*/
#ifdef DEBUG_WAYPOINTS_SEQUENCE
	for (int i=0;i<MAX_WAYPOINTS_NUMBER;++i)
		DebugNodes[i] = CommonDeclarations::GetSceneManager()->addCubeSceneNode(20,0,-1,Parent->GetScenable()->GetPosition(),Vector3(0,0,0),Vector3(1,1,1));
#endif
}

void AICommander::AddSlave(IAAObject* unit)
{
    IAI *ai = unit->GetAI();
    assert(ai);
	ai->SetUnitType(AIModule::UT_SLAVE);
	ai->SetLead(this);
	Slaves.push_back(unit);
}

bool AICommander::Parse(TiXmlElement* xml)
{
	XMLConfigurable::Parse(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int px=0, py=0, pz=0, res;	
	res = xml->QueryIntAttribute("x", &px);
	//assert(TIXML_SUCCESS==res);
	res = xml->QueryIntAttribute("y", &py);
	//assert(TIXML_SUCCESS==res);
	res = xml->QueryIntAttribute("z", &pz);
	//assert(TIXML_SUCCESS==res);

    res = xml->QueryIntAttribute("waypoints_dist", &WaypointsDistance);
    res = xml->QueryIntAttribute("fixed_waypoints", &FixedWayPoints);
    
    
	TiXmlElement *xml_element = 0;
	xml_element = xml->FirstChildElement("obj");	
	while (xml_element)
	{
		str = xml_element->Attribute("class");
		//SceneObject *obj = new SceneObject();
		SceneObject *obj;
		//PMALLOC(obj,SceneObject,SceneObject::SceneObjectAllocator);
		obj = new SceneObject;
		
        if (strcmp(str,"Dynamic Physics")==0)
		{
            obj->SetType(PT_DYNAMIC);			
		}

		obj->Parse(xml_element);			
		AddSlave(obj);

		
		int x=0, y=0, z=0;	
		res = xml_element->QueryIntAttribute("x", &x);
		//assert(TIXML_SUCCESS==res);
		res = xml_element->QueryIntAttribute("y", &y);
		//assert(TIXML_SUCCESS==res);
		res = xml_element->QueryIntAttribute("z", &z);
		//assert(TIXML_SUCCESS==res);

		//obj->Init();

		obj->SetInitialPosition(Parent->GetScenable()->GetInitialPosition()+Vector3((float)(px+x),(float)(py+y),(float)(pz+z)));
		CommonDeclarations::AddObjectRequest(obj);
		xml_element = xml_element->NextSiblingElement("obj");
	}

	return true;
}

void AICommander::RemoveSlave(IAAObject* unit)
{
	SlavesPool::iterator iPos=Slaves.begin(), iEnd=Slaves.end();
	
	for (;iPos!=iEnd;++iPos)
	{
		if (*iPos==unit)
		{
			Slaves.erase(iPos);
			break;
		}
	}
}


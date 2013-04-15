#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\aipathfinderstrategy.h"

#include "IScenable.h"
#include "Player.h"
#include "CommonDeclarations.h"

AIPathFinderStrategy::AIPathFinderStrategy(void) :
AIStrategy(ST_PATHFINDER),
//TargetObject(NULL),
TargetPoint(0,0,0),
TargetType(TT_NONE),
State(FST_NONE),
DistanceToTarget(10),
WaitTargetTimeout(100)
{
}

AIPathFinderStrategy::~AIPathFinderStrategy(void)
{
}

void AIPathFinderStrategy::SetTargetPoint(Vector3 point)
{
	State = FST_MOVING;
	TargetType = TT_POINT;
	TargetPoint=point;
}

Vector3 AIPathFinderStrategy::GetTargetPoint()
{
	return TargetPoint;
}

//void AIPathFinderStrategy::SetTargetObject(IScenable *object)
//{	
//	State = FST_MOVING;
//	TargetType = TT_OBJECT;
//	TargetObject=object;
//}
//
//IScenable *AIPathFinderStrategy::GetTargetObject()
//{
//	return TargetObject;
//}

void AIPathFinderStrategy::Init()
{
	/*if (0==TargetID)
		return;	
	IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);

	if (obj) 
	{
		SetTargetObject(obj->GetScenable());
	}*/
}

AIPathFinderStrategy::EFlyStatesType AIPathFinderStrategy::GetFlyState()
{
	return State;
}

bool AIPathFinderStrategy::Parse(TiXmlElement* xml)
{	
	if (xml == 0)
		return false; // file could not be opened

	
	//const char *str;	

	AIStrategy::Parse(xml);

	// parse the file until end reached
			
	//str = xml->Value();
	if (TargetID>=0)
	{
		SetTargetID(TargetID);		
		xml->QueryIntAttribute("dist_to_target", &DistanceToTarget);
		return true;
	}
	else
	{
		int resx, resy, resz, x, y, z;	
		resx = xml->QueryIntAttribute("x", &x);
		//assert(TIXML_SUCCESS==resx);
		resy = xml->QueryIntAttribute("y", &y);
		//assert(TIXML_SUCCESS==resy);
		resz = xml->QueryIntAttribute("z", &z);
		//assert(TIXML_SUCCESS==resz);

		if (resx==TIXML_SUCCESS && resy==TIXML_SUCCESS && resz==TIXML_SUCCESS)
		{
			Ogre::Vector3 position(x,y,z);
			SetTargetPoint(position);
		}			

		return true;			
	}

	return false;
}

void AIPathFinderStrategy::SetTargetID(int id)
{
	AIStrategy::SetTargetID(id);
	State = FST_MOVING;
	TargetType = TT_OBJECT;	
}

void AIPathFinderStrategy::Step(unsigned timeMs)
{
    Vector3 point;
    if (TargetType == TT_OBJECT) 
    {        
        Ogre::Vector3 parent_pos = Parent->GetPosition();

        if (TargetID<0)
        {
            assert(false);
            TargetPoint = parent_pos;
        }

        if (TargetID==0)
        {
            if (Owner)
            {
                TargetID = Owner->SelectTargetID();			
            }
            if (TargetID<=0)
            {
                //assert(false);
                TargetPoint = parent_pos;
            }
        }

        IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);
        //assert(obj);
        if (obj)
        {
			TargetPoint = obj->GetPosition();
            /*Ogre::Vector3 target_pos = obj->GetPosition();
            if (DistanceToTarget>0)
                TargetPoint = target_pos + (parent_pos-target_pos).normalisedCopy()*DistanceToTarget;
            else
                TargetPoint=target_pos;*/
        } else
        {
            //if (--WaitTargetTimeout<0)
            //	CommonDeclarations::DeleteObjectRequest(Parent);
            TargetPoint = parent_pos;
            //CommonDeclarations::DeleteObjectRequest(Parent);
        }
    }
}

bool AIPathFinderStrategy::IsTargetReached(Ogre::Vector3 target)
{
    /*IPhysical *phys = Parent->GetPhysical();
    assert(phys);

    Ogre::Vector3 point = target;
    Ogre::Vector3 dist = point-Parent->GetPosition();
    Ogre::Vector3 vel = phys->GetLastVelocity();
    double radius = phys->GetRadius();
    if (dist.squaredLength()>radius*radius)
        return false;*/

    Ogre::Vector3 point = target;
    Ogre::Vector3 dist = point-Parent->GetPosition();
        
    if (dist.squaredLength()>DistanceToTarget*DistanceToTarget)
        return false;

    return true;
}

void AIPathFinderStrategy::RemoveTarget()
{
	TargetType = TT_NONE;
}

AIPathFinderStrategy::ETargetType AIPathFinderStrategy::GetTargetType() const
{
	return TargetType;
}


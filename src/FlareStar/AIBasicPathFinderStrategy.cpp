#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\AiBasicPathfinderStrategy.h"
#include "CommonDeclarations.h"
#include "IAAObject.h"
#include "Room.h"
#include "RoomsManager.h"
#include "Debugging.h"
#include "IAAObject.h"
#include "IPhysical.h"

//#define PATH_FINDER_DEBUG

AIBasicPathFinderStrategy::AIBasicPathFinderStrategy(void):
AIPathFinderStrategy()
{	
}

AIBasicPathFinderStrategy::~AIBasicPathFinderStrategy(void)
{
}

void AIBasicPathFinderStrategy::Step(unsigned timeMs)
{
	// определить вектор на цель и двигаться по нему с

    // заданной скоростью.

    IPhysical *phys = Parent->GetPhysical();
    assert(phys);

	static Vector3 prev_forward_direction(0,0,0);

    AIPathFinderStrategy::Step(timeMs);

	if (TargetType != TT_NONE)
	{
        //Ogre::Vector3 point = TargetPoint;
        //Ogre::Vector3 dist = point-Parent->GetPosition();
        //Ogre::Vector3 vel = phys->GetLastVelocity();
        //if (dist.squaredLength()>vel.squaredLength() + 500.f*500.f)
        if (!IsTargetReached(TargetPoint))
        {
            Ogre::Vector3 Position = Parent->GetPosition();
            Ogre::Vector3 vr;
            vr=Position-TargetPoint;	


            Vector3 AccelerationOn(Vector3::ZERO); // = Parent->GetAccelerationOn();
            
            vr.normalise();
            phys->SetForwardDirection(vr);		
            AccelerationOn.z=-1;
            
            phys->SetAcceleration(AccelerationOn);
        }
        else
        {
            //Ogre::Vector3 point = TargetPoint;
            //Ogre::Vector3 dist = point-Parent->GetPosition();
            //Ogre::Vector3 vel = phys->GetLastVelocity();
            //double radius = phys->GetRadius();
            //if (dist.squaredLength()>radius*radius) // not full velocity step left - decrease speed
            //{
            //    phys->SetAcceleration(Ogre::Vector3::UNIT_Z);
            //} else
            {
                if (TargetType==TT_POINT)
                {
                    TargetType = TT_NONE;	
                    State = FST_POINTREACHED;
                }		
                Parent->GetPhysical()->SetThrottle(Vector3(0,0,0));		
                Parent->GetPhysical()->SetForces(Ogre::Vector3::ZERO);
                //Parent->SetVelocityVector(Vector3(0,0,0));
            }
        }
	}	
}





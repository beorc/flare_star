#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AISwarmStrategy.h"
#include "AIModule.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"
#include "Utilities.h"
#include "AIPathFinderStrategy.h"
#include "Debugging.h"
#include "IRoomable.h"

//#define FNAS1_DEBUG

AISwarmStrategy::AISwarmStrategy(void) : 
AIStrategy(),
TargetPosition(Ogre::Vector3::ZERO),
PrevOrientDirection(Ogre::Vector3::ZERO)
{
}

AISwarmStrategy::~AISwarmStrategy(void)
{
}

void AISwarmStrategy::Step(unsigned timeMs)
{		
	assert(Parent);
	assert(Owner);
    IPhysical *phys = Parent->GetPhysical();
    IScenable *scen = Parent->GetScenable();
	AIPathFinderStrategy *pf = Owner->GetPathFinder();

	float actual_rotation_fps = RotationUnit.RotationFps;
	Ogre::Vector3 orient_direction=Ogre::Vector3::ZERO;
	Ogre::Vector3 own_pos = Parent->GetPosition();

	
	if (pf)
	{                
		orient_direction = TargetPosition - own_pos;					
		Ogre::Vector3 target_point = Ogre::Vector3::NEGATIVE_UNIT_Z*10000;
		Ogre::Quaternion orientation = scen->GetOrientation();
		target_point = orientation * target_point;
		target_point = own_pos + target_point;
		pf->SetTargetPoint(target_point);                
	}
	
	
    if (!orient_direction.isZeroLength() && PrevOrientDirection != orient_direction)
    {
        Ogre::Quaternion OurOrientation = scen->GetOrientation();

        //Ogre::Vector3 direction = Target->GetPosition()-scen->GetPosition();

        orient_direction.normalise();

        Ogre::Vector3 up =CommonDeclarations::GetUpVector();
        
        Vector3 xVec = up.crossProduct(orient_direction);
        xVec.normalise();
        Vector3 yVec = orient_direction.crossProduct(xVec);
        yVec.normalise();
        Quaternion unitZToTarget = Quaternion(xVec, yVec, orient_direction);

        Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

        PrevOrientDirection = orient_direction;        
        RotationUnit.StartRotation(OurOrientation, targetOrientation, actual_rotation_fps);
    }
	
	if(RotationUnit.mRotating)
	{
		RotationUnit.Step();
		if(RotationUnit.mRotating)                                // Process timed rotation
		{				
			Ogre::Quaternion delta = RotationUnit.Slerp();	
			scen->SetOrientation(delta);
		}
	}   
}

void AISwarmStrategy::SetTargetPosition(Ogre::Vector3 pos)
{
	TargetPosition=pos;
}

bool AISwarmStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	AIStrategy::Parse(xml);

	int res;
	
	res = xml->QueryFloatAttribute("rotation_speed", &RotationUnit.RotationFps);

	return true;
}


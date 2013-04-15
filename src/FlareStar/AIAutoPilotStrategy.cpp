#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AIAutoPilotStrategy.h"
#include "AIModule.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"
#include "Utilities.h"
#include "AIPathFinderStrategy.h"
#include "Debugging.h"

//#define FNAS1_DEBUG

AIAutoPilotStrategy::AIAutoPilotStrategy(void) : 
AIStrategy(),
TargetPoint(Ogre::Vector3::ZERO),
TargetType(TT_NONE),
Active(true)
{
	Type = ST_AUTOPILOT;
}

AIAutoPilotStrategy::~AIAutoPilotStrategy(void)
{
}

void AIAutoPilotStrategy::Step(unsigned timeMs)
{
	if (!Active)
		return;

	IPhysical *phys = Parent->GetPhysical();
	IScenable *scen = Parent->GetScenable();	

	Ogre::Vector3 direction = Ogre::Vector3::ZERO;

	switch(TargetType) {
	case TT_NONE:
		{
			//return;			
		}
	case TT_INACTIVE:
		{
			Ogre::Quaternion OurOrientation = scen->GetOrientation();
			direction = OurOrientation*Ogre::Vector3::NEGATIVE_UNIT_Z;
			break;			
		}
	case TT_POINT:
		{
			AIPathFinderStrategy *pf = Owner->GetPathFinder();
			pf->SetTargetPoint(TargetPoint);
			direction = -phys->GetForwardDirection();
			break;
		}
	case TT_OBJECT:
		{
			AIPathFinderStrategy *pf = Owner->GetPathFinder();
			pf->SetTargetID(TargetID);
			direction = -phys->GetForwardDirection();
			break;
		}
	};

	
	if (!direction.isZeroLength())
	{
		Ogre::Quaternion OurOrientation = scen->GetOrientation();
		Ogre::Vector3 xVec = Ogre::Vector3::UNIT_Y.crossProduct(direction);
		xVec.normalise();
		Ogre::Vector3 yVec = direction.crossProduct(xVec);
		yVec.normalise();
		Ogre::Quaternion unitZToTarget = Ogre::Quaternion(xVec, yVec, direction);

		Ogre::Quaternion targetOrientation = Ogre::Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

		//scen->SetOrientation(targetOrientation);
		RotationUnit.StartRotation(OurOrientation, targetOrientation);
		//PrevOrientDirection = direction;
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

bool AIAutoPilotStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	AIStrategy::Parse(xml);

	//const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int res;
	
	res = xml->QueryFloatAttribute("rotation_speed", &RotationUnit.RotationFps);
    
	return TIXML_SUCCESS==res;
}

void AIAutoPilotStrategy::SetTargetPoint(Ogre::Vector3 point)
{
	TargetType = TT_POINT;
	TargetPoint = point;
}

Ogre::Vector3 AIAutoPilotStrategy::GetTargetPoint() const
{
	return TargetPoint;
}

void AIAutoPilotStrategy::SetTargetID(int id)
{
	AIStrategy::SetTargetID(id);
	TargetType = TT_OBJECT;
}

void AIAutoPilotStrategy::RemoveTarget()
{
	TargetType = TT_NONE;
}

AIAutoPilotStrategy::ETargetType AIAutoPilotStrategy::GetTargetType() const
{
	return TargetType;
}

void AIAutoPilotStrategy::SetTargetType(ETargetType type)
{
	TargetType = type;
}

void AIAutoPilotStrategy::SetActive(bool on)
{
	Active = on;
}

bool AIAutoPilotStrategy::GetActive() const
{
	return Active;
}



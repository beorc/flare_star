#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AILookStrategy.h"
#include "AIModule.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"


AILookStrategy::AILookStrategy(void) :
AIStrategy(),
LookType(LT_NONE)
{
}

AILookStrategy::~AILookStrategy(void)
{
}

void AILookStrategy::Step(unsigned timeMs)
{		
    IPhysical *phys = Parent->GetPhysical();
    IScenable *scen = Parent->GetScenable();
	
	if(RotationUnit.mRotating)
	{
		RotationUnit.Step();
		if(RotationUnit.mRotating)                                // Process timed rotation
		{				
			Ogre::Quaternion delta = RotationUnit.Slerp();	
			scen->SetOrientation(delta);
		}
	} else
	{
		Ogre::Quaternion OurOrientation = scen->GetOrientation();
		
		//Ogre::Vector3 src = OurOrientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
		Ogre::Vector3 direction = Ogre::Vector3::ZERO;
		switch(LookType) {
		case LT_FORWARD:
			direction = -phys->GetForwardDirection();
			break;
		case LT_DIRECTION:			
			direction = Direction;
			break;
		case LT_OBJECT:
			{
				if (TargetID<0)
				{
					assert(false);					
				}

				if (TargetID==0)
				{
					if (Owner)
					{
						TargetID = Owner->SelectTargetID();			
					}					
				}

				IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);
				//assert(obj);
				if (obj)
					direction = obj->GetPosition()-scen->GetPosition();			
				break;
			}
		};
				
		if (!direction.isZeroLength())
		{
			Vector3 xVec = Ogre::Vector3::UNIT_Y.crossProduct(direction);
			xVec.normalise();
			Vector3 yVec = direction.crossProduct(xVec);
			yVec.normalise();
			Quaternion unitZToTarget = Quaternion(xVec, yVec, direction);

			Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

			RotationUnit.StartRotation(OurOrientation, targetOrientation);
		}		

	}
}

bool AILookStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	const char *str;	

	// parse the file until end reached

	//str = xml->Value();

    int res;
    res = xml->QueryFloatAttribute("rotation_speed", &RotationUnit.RotationFps);

	str = xml->Attribute("type");
	if (strcmp("object",str)==0)
	{
		LookType = LT_OBJECT;		
		res = xml->QueryIntAttribute("target_id", &TargetID);
		assert(TIXML_SUCCESS==res);
		return TIXML_SUCCESS==res;

		/*str = xml->Attribute("target");		
		if (strcmp("player",str)==0)
		{
			SetTargetObject(CommonDeclarations::GetPlayer()->GetScenable());			
			return true;
		}*/
	} else
		if (strcmp("direction",str)==0)
		{	
			LookType = LT_DIRECTION;
			int resx, resy, resz, x, y, z;	
			resx = xml->QueryIntAttribute("x", &x);
			assert(TIXML_SUCCESS==resx);
			resy = xml->QueryIntAttribute("y", &y);
			assert(TIXML_SUCCESS==resy);
			resz = xml->QueryIntAttribute("z", &z);
			assert(TIXML_SUCCESS==resz);

			if (resx==TIXML_SUCCESS && resy==TIXML_SUCCESS && resz==TIXML_SUCCESS)
			{
				Ogre::Vector3 dir(x,y,z);
				SetDirection(dir);
			}			
			
			return true;			
		} else
		if (strcmp("forward",str)==0)
		{	
			LookType = LT_FORWARD;

			return true;			
		}

	
	return false;
}

void AILookStrategy::SetDirection(Ogre::Vector3 dir)
{
	LookType = LT_DIRECTION;
	Direction = dir;
}

void AILookStrategy::Init()
{
}


#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AISightNAttackStrategy.h"
#include "AIModule.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"

AISightNAttackStrategy::AISightNAttackStrategy(void) : 
AIStrategy(),
prevtime(0),
RotationSpeed(100)
{
}

AISightNAttackStrategy::~AISightNAttackStrategy(void)
{
}

void AISightNAttackStrategy::Step(unsigned timeMs)
{		
    //IPhysical *phys = Parent->GetPhysical();
    IScenable *scen = Parent->GetScenable();
	
	if (TargetID<0)
	{		
		return;
	}
	if (TargetID==0)
	{
		if (Owner)
		{
			TargetID = Owner->SelectTargetID();			
		}
		if (TargetID<=0)
		{
			TargetID=-1;
			IEquipped *eq = Parent->GetEquipped();
			eq->SetTargetPosition(Ogre::Vector3::ZERO);
			eq->SetShooting(false);
			return;
		}
	}
		
	IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);
	if (NULL==obj)
	{		
		TargetID=0;
		return;
	}
		
	IScenable* Target;	
	Target = obj->GetScenable();
	if (NULL==Target/* || phys->IsCollided()*/)
		return;

	IEquipped *eq = Parent->GetEquipped();
	Ogre::Vector3 tpos =  Target->GetPosition();
	int tradius = 300;

	IPhysical *phys = obj->GetPhysical();
	if (phys)
	{
		tpos = eq->CalculateTargetPosition(obj->GetPosition(), phys->GetLastVelocity());		
		tradius = phys->GetRadius();
	}

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
		
		Ogre::Vector3 direction = tpos-scen->GetPosition();
		direction.normalise();

		Vector3 xVec = Ogre::Vector3::UNIT_Y.crossProduct(direction);
		xVec.normalise();
		Vector3 yVec = direction.crossProduct(xVec);
		yVec.normalise();
		Quaternion unitZToTarget = Quaternion(xVec, yVec, direction);

		Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

		RotationUnit.StartRotation(OurOrientation, targetOrientation, RotationSpeed);		
	}	
	
	eq->TryToShoot(tpos, tradius);	
}

//void AISightNAttackStrategy::SetTargetObject(IScenable* obj)
//{
//	Target=obj;
//}

bool AISightNAttackStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	AIStrategy::Parse(xml);

	//const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int res;
	
	res = xml->QueryIntAttribute("rotation_speed", &RotationSpeed);

	return TIXML_SUCCESS==res;
}


#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AIInConeAttackStrategy.h"
#include "AIModule.h"
#include "AICommander.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"
#include "Debugging.h"

AIInConeAttackStrategy::AIInConeAttackStrategy(void) : 
AIStrategy(),
RotationSpeed(100),
AngleCosinusModule(0)
{
}

AIInConeAttackStrategy::~AIInConeAttackStrategy(void)
{
}

void AIInConeAttackStrategy::Step(unsigned timeMs)
{        
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
	
	if(RotationUnit.mRotating)
	{
		RotationUnit.Step();
		if(RotationUnit.mRotating)                                // Process timed rotation
		{				
			Ogre::Quaternion delta = RotationUnit.Slerp();	
			scen->SetOrientation(delta);
		}
	}// else
	{	
		int actual_rotation_speed = RotationSpeed;
		IPhysical *phys = Parent->GetPhysical();
        AICommander *commander = Owner->GetCommander();
        Ogre::Vector3 src = Ogre::Vector3::ZERO;
        if (commander)
        {
            IAAObject *obj = commander->GetParent();
            if (obj)
            {
                IScenable *scen = obj->GetScenable();
                if (scen)
                {
                    src = scen->GetOrientation()*Ogre::Vector3::NEGATIVE_UNIT_Z;
                }
            }
        } else
        {
            src = -phys->GetForwardDirection(); //OurOrientation * Ogre::Vector3::NEGATIVE_UNIT_Z;
        }
		
		Ogre::Vector3 direction = Target->GetPosition()-scen->GetPosition();
        direction.normalise();

        Ogre::Real cs = src.dotProduct(direction);
        if (src.isZeroLength() || cs>=AngleCosinusModule)
        {
			actual_rotation_speed = actual_rotation_speed/2;
			/*char log[100];
			sprintf(log,"in cone %f %f %f\n",direction.x, direction.y, direction.z);
			Debugging::Log("icas",log);*/			
        } else
		{			
			direction = src;
			/*char log[100];
			sprintf(log,"not in cone %f %f %f\n",direction.x, direction.y, direction.z);
			Debugging::Log("icas",log);*/
		}

        Ogre::Vector3 up =CommonDeclarations::GetUpVector();

		Ogre::Quaternion OurOrientation = scen->GetOrientation();
		Vector3 xVec = up.crossProduct(direction);
		xVec.normalise();
		Vector3 yVec = direction.crossProduct(xVec);
		yVec.normalise();
		Quaternion unitZToTarget = Quaternion(xVec, yVec, direction);

		Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

		RotationUnit.StartRotation(OurOrientation, targetOrientation, actual_rotation_speed);
	}

	std::pair<bool, Ogre::Real> intersection_res;
		
	IEquipped *eq = Parent->GetEquipped();
	Ogre::Ray pl_ray = eq->GetSightRay();

	intersection_res = pl_ray.intersects(Target->GetBoundingBox(true));
	if (intersection_res.first)
	{
        eq->SetTargetPosition(Target->GetPosition(), intersection_res.second);
		eq->SetShooting(true);
	} else
    {
        eq->SetTargetPosition(Ogre::Vector3::ZERO);
        eq->SetShooting(false);
    }
		
}

//void AIInConeAttackStrategy::SetTargetObject(IScenable* obj)
//{
//	Target=obj;
//}

bool AIInConeAttackStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	AIStrategy::Parse(xml);

	//const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int res;
	
	res = xml->QueryIntAttribute("rotation_speed", &RotationSpeed);

	int val;
    res = xml->QueryIntAttribute("cone_angle", &val);
    assert(TIXML_SUCCESS==res);
	if (TIXML_SUCCESS==res)
	{
		Ogre::Degree angle(val);		
		AngleCosinusModule = Ogre::Math::Cos(angle.valueRadians());
	}

	return TIXML_SUCCESS==res;
}

//void AIInConeAttackStrategy::SetAngleCosinusModule(Ogre::Real acm)
//{
//    AngleCosinusModule = acm;
//}


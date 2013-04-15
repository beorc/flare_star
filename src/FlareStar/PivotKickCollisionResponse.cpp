#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "PivotKickCollisionResponse.h"
#include "IPhysical.h"
#include "coldet.h"

PivotKickCollisionResponse::PivotKickCollisionResponse(void):
CollisionResponse()
{
}

PivotKickCollisionResponse::~PivotKickCollisionResponse(void)
{
}

PivotKickCollisionResponse::PivotKickCollisionResponse(IPhysical *owner):
CollisionResponse(owner)
{
}

void PivotKickCollisionResponse::Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner)
{
    Ogre::Real p[3];
	CollisionModel->getCollisionPoint(p,false);
	Ogre::Vector3 IntersectionPoint(p[0],p[1],p[2]);

	Ogre::Vector3 vretreat = object->Object->GetPosition()-Owner->GetPosition(), Normal;

	//object->CommitAcceleration(Vector3(0,0,0));
	//Descriptor.VelocityVector = vretreat*5;

	Normal = vretreat; //IntersectionPoint-dwith->GetPivotPos();	
	Normal.normalise();

	object->Object->AddCollisionImpulse(IntersectionPoint, Normal, &Owner->GetDescriptor());
	//object->AddForce(IntersectionPoint, vretreat*10);
	
	
	//Descriptor->ReplacingDirection=Normal*Descriptor->Throttle.length();

	//Vector3 linvel = object->GetLinVelocity();	
	//object->SetLinVelocity(Normal/2*(linvel.length()+1));
	//object->SetVelocityVector(Normal/2*linvel.length());
	//object->SetThrottle(Ogre::Vector3::ZERO);
	//object->SetThrottle(-object->GetThrottle());
    object->Throttle = -object->Throttle;

	object->Collided = true; //object->SetCollided(true);
}

void PivotKickCollisionResponse::SaveTo(TiXmlElement * root) const
{
	root->SetAttribute("collisresponse","pivotkick");
}



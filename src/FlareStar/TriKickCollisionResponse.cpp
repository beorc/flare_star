#include "StdAfx.h"


#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "TriKickCollisionResponse.h"
#include "IPhysical.h"
#include "SceneObject.h"
#include "Collidable.h"
#include "coldet.h"
#include "Debugging.h"
#include "Utilities.h"

TriKickCollisionResponse::TriKickCollisionResponse(void):
CollisionResponse()
{
}

TriKickCollisionResponse::~TriKickCollisionResponse(void)
{
}

TriKickCollisionResponse::TriKickCollisionResponse(IPhysical *owner):
CollisionResponse(owner)
{
}

void TriKickCollisionResponse::Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner)
{		
	//CollisionModel3D *OwnCollisionModel = Owner->GetOwner()->GetCollidableModule()->GetCollisionModel();

	float t1[9], t2[9], p[3], *t;

	CollisionModel->getCollisionPoint(p,false);
	Ogre::Vector3 IntersectionPoint(p[0],p[1],p[2]);

	CollisionModel->getCollidingTriangles(t1, t2, false);
	
	if (ismodelowner)
		t=t1;
	else
		t=t2;

	Ogre::Plane collidplane(Ogre::Vector3(t[0],t[1],t[2]), 
		Ogre::Vector3(t[3],t[4],t[5]),
		Ogre::Vector3(t[6],t[7],t[8]));

	double own_mass = Owner->GetMass(), obj_mass = object->Object->GetMass();

	Ogre::Vector3 linvel = object->LinVelocity;
	Ogre::Vector3 Normal = collidplane.normal;
	Ogre::Vector3 shift = (object->Object->GetPosition()-Owner->GetPosition())*own_mass/obj_mass;

	AAUtilities::Clamp(shift, 5000);

	/*char log[100];
	sprintf(log,"1 x:%f y:%f z:%f\n", shift.x, shift.y, shift.z);
	Debugging::Log("tkcr",log);*/
	
	Ogre::Vector3 vretreat = shift;
	vretreat.normalise();	
	vretreat = vretreat*Owner->GetLinVelocity();
	
	Ogre::Vector3 throttle = object->Throttle;

	object->Object->AddCollisionImpulse(IntersectionPoint, (Normal+vretreat)*Owner->GetMass()/(object->Object->GetMass()), &Owner->GetDescriptor()); //*object->GetMass()/10	

	/*if (!throttle.isZeroLength())
		object->Throttle = -throttle;	
	else
		object->Object->AddForce(IntersectionPoint, (Normal+vretreat)*Owner->GetMass()/object->Object->GetMass()); //*object->GetMass()/2
	*/
	vretreat = Normal + shift;
	//vretreat = Normal + vretreat;

	object->VelocityVector = vretreat;
	object->LinVelocity = vretreat;
	object->Throttle = Ogre::Vector3::ZERO;
	object->LinVelocityAccel = vretreat;
		
	object->Object->SetForces(Ogre::Vector3::ZERO);
	
	
	object->Collided = true; //Descriptor->Collided=true;
}

void TriKickCollisionResponse::SaveTo(TiXmlElement * root) const
{
	root->SetAttribute("collisresponse","trikick");
}

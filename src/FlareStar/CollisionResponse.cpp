#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\collisionresponse.h"
#include "IPhysical.h"

CollisionResponse::CollisionResponse(void)
{
}

CollisionResponse::CollisionResponse(IPhysical *owner)
{
	SetOwner(owner);
}

CollisionResponse::~CollisionResponse(void)
{
}

void CollisionResponse::SetOwner(IPhysical *owner)
{
	Owner = owner;
}

void CollisionResponse::Response(IPhysical* object, Ogre::Vector3 point, Ogre::Vector3 normal)
{		
    Ogre::Vector3 vretreat = object->GetPosition()-Owner->GetPosition();
    vretreat.normalise();

    object->SetForces(Ogre::Vector3::ZERO);
    object->AddCollisionImpulse(point, (normal+vretreat)*Owner->GetMass()/object->GetMass(), &Owner->GetDescriptor()); //*object->GetMass()/10	

    Ogre::Vector3 linvel = object->GetLinVelocity();	
    Ogre::Vector3 throttle = object->GetThrottle();
    if (!throttle.isZeroLength())
        object->SetThrottle(-throttle);	
    else
        object->AddForce(point, (normal+vretreat)*Owner->GetMass()/object->GetMass()); //*object->GetMass()/2

    object->SetCollided(true); //Descriptor->Collided=true;
}

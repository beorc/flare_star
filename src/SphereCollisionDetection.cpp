#include "StdAfx.h"


#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "SphereCollisionDetection.h"
#include "ICollidable.h"
#include "coldet.h"
//#include "IAffectable.h"
#include "IPhysical.h"
#include "IAAObject.h"

SphereCollisionDetection::SphereCollisionDetection(void) : 
CollisionDetection()
{
}

SphereCollisionDetection::SphereCollisionDetection(ICollidable* owner) : 
CollisionDetection(owner)
{
}

SphereCollisionDetection::~SphereCollisionDetection(void)
{
}

bool SphereCollisionDetection::Collide(ICollidable* object)
{
	bool collide=false;

	ActualCollisionModel = object->GetCollisionModel();

	object->UpdateCollisionModelTranformation();

    IPhysical *phys = Owner->GetOwner()->GetPhysical();
    assert(phys);
	Ogre::Vector3 v = phys->GetPosition();
	float origin[3] = {v.x,v.y,v.z};
	
	collide = ActualCollisionModel->sphereCollision(origin, phys->GetRadius());
	
	return collide;
}

bool SphereCollisionDetection::Collide(const Ogre::Ray &ray)
{	
    IPhysical *phys = Owner->GetOwner()->GetPhysical();
    assert(phys);
	Ogre::Vector3 o = ray.getOrigin(), d =ray.getDirection();
	float origin[3]={o.x,o.y,o.z}, direction[3]={d.x,d.y,d.z};
	float point[3];
	
	Ogre::Vector3 v = phys->GetPosition();
	float center[3] = {v.x,v.y,v.z};
    bool bres=SphereRayCollision(center, phys->GetRadius(), origin, direction, point);
    
    if (bres)
    {        
        CollisionPoint.x = point[0];
        CollisionPoint.y = point[1];
        CollisionPoint.z = point[2];
    }

	return bres;
}

void SphereCollisionDetection::SaveTo(TiXmlElement * root) const
{	
	root->SetAttribute("collisdetection", "sphere");	
}


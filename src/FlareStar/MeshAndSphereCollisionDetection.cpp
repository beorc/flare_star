#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "meshandspherecollisiondetection.h"
#include "ICollidable.h"
#include "IAAObject.h"
#include "IPhysical.h"
#include "coldet.h"

MeshAndSphereCollisionDetection::MeshAndSphereCollisionDetection(void) : 
CollisionDetection(),
CachedCollisions(0)
{
}

MeshAndSphereCollisionDetection::MeshAndSphereCollisionDetection(ICollidable* owner) : 
CollisionDetection(owner),
CachedCollisions(0)
{
}

MeshAndSphereCollisionDetection::~MeshAndSphereCollisionDetection(void)
{
}

bool MeshAndSphereCollisionDetection::Collide(ICollidable* object)
{
	bool collide=false;
	
	ActualCollisionModel = Owner->GetCollisionModel();
	Owner->UpdateCollisionModelTranformation();
    
    IPhysical *phys = object->GetOwner()->GetPhysical();
    assert(phys);

	if (CachedCollisions>3) 
	{
		Ogre::Vector3 v = phys->GetPosition();
		float origin[3] = {v.x,v.y,v.z},
			collide = ActualCollisionModel->sphereCollision(origin, phys->GetRadius());// collision(WithCollisionModel, -1, 0, m);		
	} else
	{
		CollisionModel3D *WithCollisionModel = object->GetCollisionModel();
		if (WithCollisionModel==ActualCollisionModel) 
		{
			float m[16];
			object->GetCollisionModelTranformation(m);
			collide = ActualCollisionModel->collision(WithCollisionModel, -1, 0, m);		
		} else
		{
			object->UpdateCollisionModelTranformation();
			collide = ActualCollisionModel->collision(WithCollisionModel);
		}
		if (collide) 
		{
			++CachedCollisions;
		}
	}		

	if (!collide)
		CachedCollisions=0;

	return collide;
}

bool MeshAndSphereCollisionDetection::Collide(const Ogre::Ray &ray)
{	
	Ogre::Vector3 o = ray.getOrigin(), d =ray.getDirection();
	float origin[3]={o.x,o.y,o.z}, direction[3]={d.x,d.y,d.z};	
	float segmax = d.length();	

	Owner->UpdateCollisionModelTranformation();

    bool bres = Owner->GetCollisionModel()->rayCollision(origin, direction, false, 0.0f, segmax);
    if (bres)
    {
        float colpoint[3];
        Owner->GetCollisionModel()->getCollisionPoint(colpoint,false);
        CollisionPoint.x = colpoint[0];
        CollisionPoint.y = colpoint[1];
        CollisionPoint.z = colpoint[2];
    }   
	return bres;	
}

void MeshAndSphereCollisionDetection::SaveTo(TiXmlElement * root) const
{	
	root->SetAttribute("collisdetection", "mesh_sphere");	
}


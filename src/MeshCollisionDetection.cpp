#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "MeshCollisionDetection.h"
#include "ICollidable.h"
#include "coldet.h"

MeshCollisionDetection::MeshCollisionDetection(void) : 
CollisionDetection()
{
}

MeshCollisionDetection::MeshCollisionDetection(ICollidable* owner) : 
CollisionDetection(owner)
{
}

MeshCollisionDetection::~MeshCollisionDetection(void)
{
}

bool MeshCollisionDetection::Collide(ICollidable* object)
{
	bool collide=false;

	ActualCollisionModel = Owner->GetCollisionModel();
	Owner->UpdateCollisionModelTranformation();
	
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

	return collide;
}

bool MeshCollisionDetection::Collide(const Ogre::Ray &ray)
{	
	Ogre::Vector3 o = ray.getOrigin(), d =ray.getDirection();
    float segmax = d.length();
    d.normalise();
	float origin[3]={o.x,o.y,o.z}, direction[3]={d.x,d.y,d.z};

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

void MeshCollisionDetection::SaveTo(TiXmlElement * root) const
{	
	root->SetAttribute("collisdetection", "mesh");	
}



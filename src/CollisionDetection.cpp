#include "StdAfx.h"


#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\collisiondetection.h"
#include "ICollidable.h"
#include "coldet.h"

CollisionDetection::CollisionDetection(void):
ActualCollisionModel(0)
{
}

CollisionDetection::CollisionDetection(ICollidable *owner)
{
	SetOwner(owner);
}

CollisionDetection::~CollisionDetection(void)
{
}


void CollisionDetection::SetOwner(ICollidable *owner)
{
	Owner = owner;
}

CollisionModel3D *CollisionDetection::GetActualCollisionModel()
{
	return ActualCollisionModel;
}

Ogre::Vector3  CollisionDetection::GetCollisionPoint()
{
    return CollisionPoint;
}

bool CollisionDetection::Collide(const Ogre::Vector3 &point, const float &radius)
{
	float origin[3]={point.x,point.y,point.z};

	Owner->UpdateCollisionModelTranformation();

	bool bres = Owner->GetCollisionModel()->sphereCollision(origin, radius);

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

#include "StdAfx.h"


#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "slidecollisionresponse.h"
#include "IPhysical.h"
#include "ICollidable.h"
#include "coldet.h"

SlideCollisionResponse::SlideCollisionResponse(void):
CollisionResponse()
{
}

SlideCollisionResponse::~SlideCollisionResponse(void)
{
}

SlideCollisionResponse::SlideCollisionResponse(IPhysical *owner):
CollisionResponse(owner)
{
}

void SlideCollisionResponse::Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner)
{
	// при коллизии определяется треугольник и объект движется параллельно плоскости тругольника
	
	float t1[9], t2[9], p[3], *t;

	CollisionModel->getCollisionPoint(p,false);
	Ogre::Vector3 normal, IntersectionPoint(p[0],p[1],p[2]);

	CollisionModel->getCollidingTriangles(t1, t2, false);

	if (ismodelowner)
		t=t1;
	else
		t=t2;
	
	Ogre::Plane collidplane(Ogre::Vector3(t[0],t[1],t[2]), 
		Ogre::Vector3(t[3],t[4],t[5]),
		Ogre::Vector3(t[6],t[7],t[8]));

	Ogre::Vector3 proj = collidplane.projectVector(object->LinVelocity);
	proj.normalise();	
	Ogre::Vector3 Normal = collidplane.normal;

	Ogre::Vector3 linvel = object->LinVelocity;	

	Ogre::Vector3 throttle = object->Throttle;

	Ogre::Vector3 dir=proj+Normal/2;
	dir.normalise();    
    object->Object->SetForces(Ogre::Vector3::ZERO);

	object->VelocityVector = dir*(linvel.length()+1);
	object->LinVelocity = dir*(linvel.length()+1);

	object->Object->SetReplacingDirection(dir*(linvel.length()+1));

	object->Object->AddForce(IntersectionPoint, dir*(linvel.length()+1)*Owner->GetMass()/object->Object->GetMass());
}

void SlideCollisionResponse::SaveTo(TiXmlElement * root) const
{
	root->SetAttribute("collisresponse","slide");
}

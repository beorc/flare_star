#pragma once
#include "collisiondetection.h"

class MeshAndSphereCollisionDetection :
	public CollisionDetection
{
public:
	MeshAndSphereCollisionDetection(void);
	MeshAndSphereCollisionDetection(ICollidable* owner);
	virtual ~MeshAndSphereCollisionDetection(void);

	bool Collide(ICollidable* object);
	bool Collide(const Ogre::Ray &ray);

	virtual void SaveTo(TiXmlElement * root) const;
protected:
	char CachedCollisions;
};

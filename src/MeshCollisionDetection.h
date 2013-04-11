#pragma once
#include "collisiondetection.h"

class MeshCollisionDetection :
	public CollisionDetection
{
public:
	MeshCollisionDetection(void);
	MeshCollisionDetection(ICollidable* owner);
	virtual ~MeshCollisionDetection(void);

	bool Collide(ICollidable* object);
	bool Collide(const Ogre::Ray &ray);

	virtual void SaveTo(TiXmlElement * root) const;
protected:
};

#pragma once
#include "collisiondetection.h"

class SphereCollisionDetection :
	public CollisionDetection
{
public:
	SphereCollisionDetection(void);
	SphereCollisionDetection(ICollidable* owner);
	virtual ~SphereCollisionDetection(void);

	bool Collide(ICollidable* object);
	bool Collide(const Ogre::Ray &ray);

	virtual void SaveTo(TiXmlElement * root) const;
protected:
};

#pragma once
#include <Ogre.h>
#include "ISerializable.h"

class ICollidable;
class CollisionModel3D;

class CollisionDetection : public ISerializable
{
public:
	CollisionDetection(void);
	CollisionDetection(ICollidable *owner);
	virtual ~CollisionDetection(void);

	void SetOwner(ICollidable *owner);
	CollisionModel3D *GetActualCollisionModel();
	virtual bool Collide(ICollidable* object) = 0;
	virtual bool Collide(const Ogre::Ray &ray) = 0;
	virtual bool Collide(const Ogre::Vector3 &point, const float &radius);
    Ogre::Vector3   GetCollisionPoint();
protected:
	ICollidable* Owner;
	CollisionModel3D *ActualCollisionModel;
    Ogre::Vector3    CollisionPoint;
};

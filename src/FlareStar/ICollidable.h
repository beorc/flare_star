#pragma once

#include <Ogre.h>
#include "ISerializable.h"

class CollisionDetection;
class CollisionModel3D;
class IAAObject;

class ICollidable : public ISerializable
{
public:        
	virtual bool GetCollising() const = 0;
	//virtual void    InitCollisionModel() = 0;
	virtual bool	Collide(ICollidable* object) = 0;	
	virtual bool	Collide(const Ogre::Ray &ray) = 0;
	virtual bool	Collide(const Ogre::Vector3 &origin, const float &radius) = 0;

	virtual void AddCollision(ICollidable *obj) = 0;
	virtual void ClearCollisions() = 0;
    virtual bool HaveCollisions() const = 0;
	virtual bool IsInCollisions(ICollidable *obj) = 0;
	virtual void UpdateCollisionModelTranformation() = 0;
	virtual void GetCollisionModelTranformation(float m[]) = 0;
	virtual CollisionModel3D * GetCollisionModel() = 0;
	
	virtual CollisionDetection *GetCollisionDetection() const = 0;
	virtual void SetCollisionDetection(CollisionDetection *module) = 0;
	
	virtual const IAAObject *GetOwner() const = 0;
	virtual char *GetCollisionModelname() const = 0;
};

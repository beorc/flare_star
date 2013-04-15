#pragma once
#include "ISerializable.h"

class IPhysical;
class CollisionModel3D;
struct PhysObjDescriptor;

class CollisionResponse : public ISerializable
{
public:
	CollisionResponse(void);
	CollisionResponse(IPhysical*);
	virtual ~CollisionResponse(void);
	
	void SetOwner(IPhysical *owner);
	virtual void Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner) = 0;
    void Response(IPhysical* object, Ogre::Vector3 point, Ogre::Vector3 normal);
protected:
	IPhysical *Owner;
};

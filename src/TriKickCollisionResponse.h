#pragma once
#include "collisionresponse.h"

class TriKickCollisionResponse :
	public CollisionResponse
{
public:
	TriKickCollisionResponse(void);
	virtual ~TriKickCollisionResponse(void);

	TriKickCollisionResponse(IPhysical*);	
	void Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner);
    //void Response(IPhysical* object, Ogre::Vector3 point, Ogre::Vector3 normal);
	virtual void SaveTo(TiXmlElement * root) const;
};

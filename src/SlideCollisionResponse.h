#pragma once
#include "collisionresponse.h"

class SlideCollisionResponse :
	public CollisionResponse
{
public:
	SlideCollisionResponse(void);
	virtual ~SlideCollisionResponse(void);

	SlideCollisionResponse(IPhysical*);	
	void Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner);
	virtual void SaveTo(TiXmlElement * root) const;
};

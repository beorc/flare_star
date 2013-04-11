#pragma once
#include "collisionresponse.h"

class PivotKickCollisionResponse :
	public CollisionResponse
{
public:
	PivotKickCollisionResponse(void);
	virtual ~PivotKickCollisionResponse(void);

	PivotKickCollisionResponse(IPhysical*);	
	void Response(PhysObjDescriptor* object, CollisionModel3D *CollisionModel, bool ismodelowner);
	virtual void SaveTo(TiXmlElement * root) const;
};

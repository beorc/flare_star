#pragma once

#include "XMLConfigurable.h"
//#include "IAAObject.h"
#include "Waypoint.h"
#include "IPhysical.h"

#include <Ogre.h>

#include <vector>

class IAAObject;
class CollisionResponse;

class Physical : public XMLConfigurable, public IPhysical
{
public:		
	typedef std::vector<Waypoint> WaypointsPool;

	Physical(void);	
	virtual ~Physical(void);
		
	PhysObjDescriptor GetDescriptor() const;
	void SetDescriptor(PhysObjDescriptor descr);
	
	virtual void	InitPhysics();
		
	WaypointsPool *GetWaypoints();
		
	virtual Ogre::Vector3 GetLastVelocity() const;
		
	virtual void Update(float dt);
	void UpdateMatrices();	
	virtual	void AddCollisionImpulse(Vector3 P, Vector3 N, PhysObjDescriptor *dwith=NULL);

	void CalcVelocityVector(unsigned diff);
	
	virtual bool Parse(TiXmlElement* xml);
	
	virtual double GetRadius() const;

    virtual void AddForce(Vector3 P, Vector3 F);	
    virtual void SetThrottle(Ogre::Vector3);    
    virtual bool IsCollided() const;

    virtual Vector3 GetLinVelocity() const;	
	virtual void SetRotVelocity(Ogre::Vector3 vel);
    virtual Vector3 GetRotVelocity() const;
	virtual Vector3 GetLinAccelearation() const;	
    virtual void SetCollided(bool);
	virtual void SetForwardDirection(Ogre::Vector3 acceleration);
	virtual Ogre::Vector3 GetForwardDirection();
	
	virtual void SetAcceleration(Ogre::Vector3 acceleration);

	virtual void SetForces(Ogre::Vector3);
	virtual Ogre::Vector3 GetThrottle() const;

	/*void SetMaxVelocity(int vel);
	int GetMaxVelocity() const;*/

	virtual double GetMass() const;

	virtual void SetLinDamping(float damping);
	virtual void SetRotDamping(float damping);

    void SetOwner(IAAObject* owner);
    IAAObject *GetOwner();

    Ogre::Vector3 GetPosition() const;
	Ogre::Vector3 GetPrevPosition() const;
	
    virtual void SetPosition(Ogre::Vector3);

    virtual CollisionResponse *GetCollisionResponse();
	virtual Ogre::Vector3 GetAccelerationOn() const;

	virtual void SetReplacingDirection(Ogre::Vector3 direction);
	virtual void Stop();

	virtual void SetVelocityVector(Ogre::Vector3);
	virtual Ogre::Vector3 GetVelocityVector() const;
	
	virtual void SetLinVelocity(Ogre::Vector3);
	void SetLinVelocityAccel(Ogre::Vector3);

	virtual void Step(unsigned);

	virtual void SaveTo(TiXmlElement * root) const;

	Ogre::Matrix3 GetBoxInertia() const;

    void SetMaxVelocities(Ogre::Vector3 vel);
    Ogre::Vector3 GetMaxVelocities() const;

protected:
	PhysObjDescriptor Descriptor;

	//AI
	WaypointsPool Waypoints;
	
	//Physics
	Vector3 Size;			// size of the box
	double Radius;
		
	Ogre::Matrix3 Orientation;	// orientation of the body
	
	double  Mass;			// mass of box
	Ogre::Matrix3 BoxInertia;		// inertia of box
	Ogre::Matrix3 InvInertia;		// orientated inverse inertia

	//Vector3 LinVelocity, LinVelocityBase, LinVelocityAccel;		// linear velocity
	//Vector3 RotVelocity;		// rotational velocity
	//Vector3 LinAcceleration;	// linear acceleration
	//Vector3 RotAcceleration;	// rotational acceleration

	Vector3 Forces;			// forces computed for this frame
	Vector3 Torques;		// torques computed for this frame

	Vector3 AccelerationOn;

	float LinDamping, RotDamping;
	//int MaxVelocity;
    Ogre::Vector3 MaxVelocities, VelocitiesStep;

    Ogre::Vector3 Position, PrevPosition;

    IAAObject *Owner;

    std::auto_ptr<CollisionResponse> CollisionResponseModule;
};

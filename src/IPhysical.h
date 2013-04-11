#pragma once
#include <Ogre.h>
#include "ISerializable.h"

class CollisionResponse;
class IAAObject;
class IPhysical;

struct PhysObjDescriptor {
		PhysObjDescriptor() : 		
		Throttle(0,0,0),
		Collided(false),		
		VelocityVector(0,0,0),				
		ForwardDirection(0,0,0),
		ReplacingDirection(0,0,0),
		Collising(false),
		LinVelocityBase(Ogre::Vector3::ZERO),
		LinVelocityAccel(Ogre::Vector3::ZERO),
		Object(NULL),
		LastVelocity(Ogre::Vector3::ZERO)
		{
		}

		Ogre::Vector3				Throttle;
	
		Ogre::Vector3		ForwardDirection, ReplacingDirection;	// velocity in earth coordinates	

		Ogre::Vector3		VelocityVector;
		
		bool	Collided, Collising;

		//

		Ogre::Vector3 LinVelocity, LinVelocityBase, LinVelocityAccel;		// linear velocity
		Ogre::Vector3 RotVelocity;		// rotational velocity
		Ogre::Vector3 LinAcceleration;	// linear acceleration
		Ogre::Vector3 RotAcceleration;	// rotational acceleration
		Ogre::Vector3 LastVelocity;

		IPhysical *Object;
	};

class IPhysical : public ISerializable
{
public:
    virtual Ogre::Vector3 GetPosition() const = 0;
	virtual Ogre::Vector3 GetPrevPosition() const = 0;
    virtual bool IsCollided() const = 0;
    virtual void SetCollided(bool) = 0;        
    virtual void SetThrottle(Ogre::Vector3) = 0;    
    virtual void SetForces(Ogre::Vector3) = 0;
    virtual Ogre::Vector3 GetThrottle() const = 0;
    virtual Ogre::Vector3 GetLinVelocity() const = 0;	    
    virtual Ogre::Vector3 GetRotVelocity() const = 0;
	virtual void SetRotVelocity(Ogre::Vector3 vel) = 0;
    virtual void SetAcceleration(Ogre::Vector3 acceleration) = 0;    
    virtual void SetForwardDirection(Ogre::Vector3 acceleration) = 0;    
    virtual void SetLinDamping(float damping) = 0;
    virtual void SetRotDamping(float damping) = 0;
    virtual void AddForce(Ogre::Vector3 P, Ogre::Vector3 F) = 0;    
    virtual double GetMass() const = 0;
    virtual double GetRadius() const = 0;
	virtual	CollisionResponse *GetCollisionResponse() = 0;
	virtual void SetReplacingDirection(Ogre::Vector3 direction) = 0;
	virtual void Stop() = 0;
	virtual void SetVelocityVector(Ogre::Vector3) = 0;
	virtual Ogre::Vector3 GetVelocityVector() const = 0;
	virtual void SetLinVelocity(Ogre::Vector3) = 0;
	

	virtual void AddCollisionImpulse(Ogre::Vector3 P, Ogre::Vector3 N, PhysObjDescriptor *dwith=NULL) = 0;
	virtual Ogre::Vector3 GetForwardDirection() = 0;
	virtual void Step(unsigned) = 0;
	virtual void SetPosition(Ogre::Vector3) = 0;
	virtual Ogre::Vector3 GetAccelerationOn() const = 0;
	//virtual void SetMaxVelocity(int vel) = 0;
	//virtual int GetMaxVelocity() const = 0;
    
    virtual void SetMaxVelocities(Ogre::Vector3 vel) = 0;
    virtual Ogre::Vector3 GetMaxVelocities() const = 0;

    virtual void	InitPhysics() = 0;
	virtual void SetLinVelocityAccel(Ogre::Vector3) = 0;
	virtual Ogre::Vector3 GetLastVelocity() const = 0;

	virtual PhysObjDescriptor GetDescriptor() const = 0;
	virtual void SetDescriptor(PhysObjDescriptor descr) = 0;

	virtual Ogre::Vector3 GetLinAccelearation() const = 0;
	virtual Ogre::Matrix3 GetBoxInertia() const = 0;
};

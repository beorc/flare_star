#pragma once
#include <Ogre.h>

class Room;
class AIModule;

class IAffectable
{
public:	
    virtual Ogre::Vector3 GetPivotPos() const = 0;
    virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed=false) = 0;
    virtual bool IsCollided() = 0;
    virtual void SetCollided(bool) = 0;        
    virtual void SetThrottle(Ogre::Vector3) = 0;    
	virtual void SetForces(Ogre::Vector3) = 0;
	virtual Ogre::Vector3 GetThrottle() = 0;
    virtual Ogre::Vector3 GetLinVelocity() = 0;	    
    virtual Ogre::Vector3 *GetRotVelocity() = 0;
    virtual void SetDirection(Ogre::Vector3 direction) = 0;
    virtual void SetAcceleration(Ogre::Vector3 acceleration) = 0;    
	virtual Ogre::Quaternion GetOrientation() = 0;
	virtual Ogre::Quaternion GetAbsoluteOrientation() const = 0;
	virtual Ogre::Vector3 GetScale() const = 0;
	virtual void SetOrientation(Ogre::Quaternion direction) = 0;
	virtual void SetForwardDirection(Ogre::Vector3 acceleration) = 0;
	virtual Room *GetRoom(Ogre::Vector3 &point) = 0;
	virtual AIModule *GetAiModule() = 0;
    virtual int GetAffectableID() = 0;
    virtual void SetAffectableID(int id) = 0;
	virtual void SetScriptValue(int index, char val) = 0;
	virtual char GetScriptValue(int index) = 0;
	virtual void SetLinDamping(float damping) = 0;
	virtual void SetRotDamping(float damping) = 0;
	virtual void AddForce(Ogre::Vector3 P, Ogre::Vector3 F) = 0;
	virtual void Destroy() = 0;
	virtual double GetRadius() const = 0;
	virtual double GetMass() const = 0;
};

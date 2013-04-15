#pragma once

#include <Ogre.h>

enum EObjType {PT_STATIC, PT_DYNAMIC, PT_NOT_PHYSYCS_DYNAMIC};

class IScenable
{
public:    
    virtual Ogre::Vector3 GetPosition() const = 0;
    virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed=false) const = 0;    
    virtual void SetDirection(Ogre::Vector3 direction) = 0;    
    virtual Ogre::Quaternion GetOrientation() = 0;
    virtual Ogre::Quaternion GetAbsoluteOrientation() const = 0;
    virtual Ogre::Vector3 GetScale() const = 0;
    virtual void SetOrientation(Ogre::Quaternion direction) = 0;   
	virtual Ogre::Vector3 GetInitialPosition() const = 0;
	virtual Ogre::Quaternion GetInitialRotation() const = 0;

	virtual EObjType GetType() const = 0;
	virtual void SetType(EObjType type)  = 0;

	virtual int GetTag() const = 0;
	virtual void SetTag(int tag)  = 0;

	virtual void GetBoundingPoints(float Side, Ogre::Vector3 outPoints[]) = 0;
	//virtual void RestoreBackupPosition() = 0;

	virtual Ogre::SceneNode *GetNode() const = 0;
	virtual void SetEntity(Ogre::MovableObject *) = 0;
    virtual Ogre::Vector3 GetPrevPosition() const = 0;
	virtual void SetInitialPosition(Ogre::Vector3 pos) = 0;
	virtual void SetInitialDirection(Ogre::Vector3 dir) = 0;
	virtual bool AddEffect(int lid, bool need_init) = 0;
	virtual bool RemoveEffect(int lid) = 0;
	//virtual bool RemoveEffect(const char*) = 0;

	virtual void SetPosition(Ogre::Vector3 pos) = 0;
	virtual const char *GetModelName() const = 0;
};

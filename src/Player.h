#pragma once

#include "SceneObject.h"
//#include "Gunned.h"

#include "InterpolatedRotation.h"

class Core;
class Weapon;
class Equipment;
class AIPathFinderStrategy;

class Player : public SceneObject//, public Gunned
{
public:
	Player(void);	
	~Player(void);

	//virtual void AddEquipment(Equipment* equipment);
	virtual bool AddEquipment(Equipment* equipment);

	virtual void ChooseBestWeapon();
	virtual void SetShooting(bool state);

	virtual bool Parse(TiXmlElement* xml);	
	virtual void Step(unsigned timeMs);
	virtual bool Killed();

	virtual Ogre::Quaternion GetOrientation();
	virtual void SetOrientation(Ogre::Quaternion direction);
	void HandleCameraCollision();
    //bool RestoreBackupPosition();

	Ogre::Vector3 GetDirection();

	virtual void SetActive(bool on);

	virtual void SaveTo(TiXmlElement * root) const;

	void SetAutoPilotActive(bool on);
	bool GetAutoPilotActive() const;

	virtual void Register();

    //virtual bool InitEntity();
private:
	Weapon *ActiveWeapon;
	Ogre::Quaternion RelativeOrientation, StrafeRelativeOrientation, LookRelativeOrientation;    

	InterpolatedRotation StrafeRelativeRotationUnit, LookRelativeRotationUnit;
	
	bool AutoPilotActive;

	float StrafeRollSpeed, LookRollSpeed, StrafeFps, LookFps;
	float MaxStrafeRollAngle, MaxLookRollAngle, StrafeBufferAngle, LookBufferAngle;

	
    //Ogre::Vector3 BackupPosition;
    //unsigned BackupPosTime;
	//unsigned LastTimes[1];
};

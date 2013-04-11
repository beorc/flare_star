#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\player.h"
//#include "PhysicsEngine.h"
#include "Debugging.h"
#include "Weapon.h"

//#include "Core.h"
#include "CommonDeclarations.h"
#include "EquippedObject.h"

#include "coldet.h"
#include "CollisionDetection.h"
#include "Room.h"
#include "RoomsManager.h"
#include "IRoomable.h"
#include "IPhysical.h"
#include "AIAutoPilotStrategy.h"
#include "GUIRadarLayout.h"
#include "GUISystem.h"
#include "BonusesKeeper.h"
#include "AACamera.h"
#include "HUDCentre.h"


Player::Player(void) :
SceneObject(),
ActiveWeapon(NULL),
RelativeOrientation(Quaternion::IDENTITY),
StrafeRelativeOrientation(Quaternion::IDENTITY),
LookRelativeOrientation(Quaternion::IDENTITY),
AutoPilotActive(false),
MaxStrafeRollAngle(0),
MaxLookRollAngle(0),
StrafeRollSpeed(0),
LookRollSpeed(0),
StrafeBufferAngle(0),
LookBufferAngle(0),
StrafeFps(0),
LookFps(0)
{	
    Type = PT_DYNAMIC;
	Tag = T_PLAYER;	
}

Player::~Player(void)
{
}

bool Player::AddEquipment(Equipment* equipment)
{
	bool res = false;
	if (EquippedStaff.get())
	{
		res = EquippedStaff->AddEquipment(equipment);

		if (equipment->GetType()==Weapon::ET_WEAPON)
			ActiveWeapon = static_cast<Weapon*>(equipment);
	}
	return res;
}

bool Player::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	
	str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}

    SceneObject::Parse(xml);

	str = xml->Value();
	assert(strcmp(str,"player")==0);
	
	int x, y, z, resx, resy, resz;	
	resx = xml->QueryIntAttribute("x", &x);
	//assert(TIXML_SUCCESS==res);
	resy = xml->QueryIntAttribute("y", &y);
	//assert(TIXML_SUCCESS==res);
	resz = xml->QueryIntAttribute("z", &z);
	//assert(TIXML_SUCCESS==res);

	//Init();

	if (TIXML_SUCCESS==resx && TIXML_SUCCESS==resy && TIXML_SUCCESS==resz)
		SetInitialPosition(Vector3((float)x,(float)y,(float)z));

	TiXmlElement *game_config = CommonDeclarations::GetGameConfig();
	assert(game_config);
	if (game_config)
	{
		TiXmlElement *xml_element = NULL, *player_cfg=NULL;
		player_cfg = game_config->FirstChildElement("player");
		assert(player_cfg);

		xml_element = player_cfg->FirstChildElement("strafe_roll");
		assert(xml_element);

		if (xml_element)
		{
			int res;
			res = xml_element->QueryFloatAttribute("roll_fps",&StrafeFps);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryFloatAttribute("max_roll_angle",&MaxStrafeRollAngle);
			assert(TIXML_SUCCESS==res);
			
			MaxStrafeRollAngle = Ogre::Math::DegreesToRadians(MaxStrafeRollAngle);

			res = xml_element->QueryFloatAttribute("roll_speed",&StrafeRollSpeed);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryFloatAttribute("angle_buffer",&StrafeBufferAngle);
			assert(TIXML_SUCCESS==res);			

			StrafeBufferAngle = Ogre::Math::DegreesToRadians(StrafeBufferAngle);
		}

		xml_element = player_cfg->FirstChildElement("look_roll");
		assert(xml_element);

		if (xml_element)
		{
			int res;
			
			res = xml_element->QueryFloatAttribute("roll_fps",&LookFps);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryFloatAttribute("max_roll_angle",&MaxLookRollAngle);
			assert(TIXML_SUCCESS==res);

			MaxLookRollAngle = Ogre::Math::DegreesToRadians(MaxLookRollAngle);

			res = xml_element->QueryFloatAttribute("roll_speed",&LookRollSpeed);
			assert(TIXML_SUCCESS==res);
			
			res = xml_element->QueryFloatAttribute("angle_buffer",&LookBufferAngle);
			assert(TIXML_SUCCESS==res);

			LookBufferAngle = Ogre::Math::DegreesToRadians(LookBufferAngle);
		}
	}
	
	return true;
}

void Player::ChooseBestWeapon()
{
}

void Player::SetShooting(bool state)
{
	if (NULL!=EquippedStaff.get())
	{
		EquippedStaff->SetShooting(state);		
	}	
}

//bool Player::RestoreBackupPosition()
//{
//	IPhysical *object = GetPhysical();
//	object->SetPosition(BackupPosition);
//
//	/*object->SetVelocityVector(Ogre::Vector3::ZERO);
//	object->SetLinVelocity(Ogre::Vector3::ZERO);
//	object->SetThrottle(Ogre::Vector3::ZERO);
//	object->SetLinVelocityAccel(Ogre::Vector3::ZERO);*/
//
//	object->Stop();
//	return true;
//}

void Player::Step(unsigned timeMs)
{
    //Ogre::Quaternion InvInitialOrientation = InitialRotation.Inverse();
	if (!AutoPilotActive)
		GetPhysical()->SetForwardDirection(GetOrientation()*Vector3::UNIT_Z);

	//if (GetRoomable()->GetRooms()->IsEmpty())
	//{
	//	HUDCentre::GetInstance()->ShowGuiltyConscienceOverlay(true);		
	//} else
	//{
	//	HUDCentre::GetInstance()->ShowGuiltyConscienceOverlay(false);		
	//}

    //int diff = timeMs - BackupPosTime;
    /*BackupPosTime+=timeMs;
    if (BackupPosTime>BACKUP_POSITION_TIMEOUT && !GetRoomable()->GetRooms()->IsEmpty())
    {        
		BackupPosTime = 0;
		if (CommonDeclarations::GetRoomManager()->IsValidPosition(GetPhysical()->GetPosition()))
			BackupPosition = GetPhysical()->GetPosition();
    }*/

	//static unsigned RollSpeed = 10;
	SceneObject::Step(timeMs);

	if (BonusesModule.get())
		BonusesModule->Step(timeMs);
    
    Ogre::Vector3 AccelerationOn = GetPhysical()->GetAccelerationOn();

	Ogre::Vector3 rot_velocity = CommonDeclarations::GetCamera()->GetRotVelocity();
	
	float angle = abs(LookRelativeOrientation.getRoll().valueRadians());
	float actual_fps = LookFps;
	
	if (angle>0 && angle<LookBufferAngle)
	{
		float coeff = 2 + 2*angle/LookBufferAngle;		
		actual_fps*=coeff;
	}

	if (angle>MaxLookRollAngle-LookBufferAngle)
	{
		float coeff = 2 + 2*(MaxLookRollAngle-LookBufferAngle)/angle;		
		actual_fps*=coeff;
	}

	if (rot_velocity.x>1)
	{
		if (angle<MaxLookRollAngle)
		{
			Quaternion q;
			q.FromAngleAxis(Ogre::Degree(LookRollSpeed),Vector3::UNIT_Z);
			LookRelativeRotationUnit.StartRotation(LookRelativeOrientation, LookRelativeOrientation*q, actual_fps);
		}
	} else
		if (rot_velocity.x<-1)
		{
			if (angle<MaxLookRollAngle)
			{
				Quaternion q;
				q.FromAngleAxis(-Ogre::Degree(LookRollSpeed),Vector3::UNIT_Z);
				LookRelativeRotationUnit.StartRotation(LookRelativeOrientation, LookRelativeOrientation*q, actual_fps);
			}
		} else
			{
				if (LookRelativeOrientation!=Quaternion::IDENTITY)			
				{
					Quaternion q = Quaternion::IDENTITY;
					LookRelativeRotationUnit.StartRotation(LookRelativeOrientation, q, actual_fps);
				}
			}
	
	angle = abs(StrafeRelativeOrientation.getRoll().valueRadians());	
	actual_fps = StrafeFps;
	
	if (angle>0 && angle<StrafeBufferAngle)
	{
		float coeff = 2 + 2*angle/StrafeBufferAngle;		
		actual_fps*=coeff;
	}

	if (angle>MaxStrafeRollAngle-StrafeBufferAngle)
	{
		float coeff = 2 + 2*(MaxStrafeRollAngle-StrafeBufferAngle)/angle;		
		actual_fps*=coeff;
	}

	if (1==AccelerationOn.x)
	{		
		if (angle<MaxStrafeRollAngle)
		{
			Quaternion q;
			q.FromAngleAxis(-Ogre::Degree(StrafeRollSpeed),Vector3::UNIT_Z);			
			StrafeRelativeRotationUnit.StartRotation(StrafeRelativeOrientation, StrafeRelativeOrientation*q, actual_fps);			
		}		
	}
	else
		if(-1==AccelerationOn.x)
		{
			if (angle<MaxStrafeRollAngle)
			{
				Quaternion q;
				q.FromAngleAxis(Ogre::Degree(StrafeRollSpeed),Vector3::UNIT_Z);				
				StrafeRelativeRotationUnit.StartRotation(StrafeRelativeOrientation, StrafeRelativeOrientation*q, actual_fps);
			}			
		}
		else
		{
			if (StrafeRelativeOrientation!=Quaternion::IDENTITY)			
			{
				Quaternion q = Quaternion::IDENTITY;
				StrafeRelativeRotationUnit.StartRotation(StrafeRelativeOrientation, q, actual_fps);
			}
		}


		{	
			if(StrafeRelativeRotationUnit.mRotating)
			{
				StrafeRelativeRotationUnit.Step();
				if(StrafeRelativeRotationUnit.mRotating)                                // Process timed rotation
				{				
					StrafeRelativeOrientation = StrafeRelativeRotationUnit.Slerp();					
				}
			}
			if(LookRelativeRotationUnit.mRotating)
			{
				LookRelativeRotationUnit.Step();
				if(LookRelativeRotationUnit.mRotating)                                // Process timed rotation
				{				
					LookRelativeOrientation = LookRelativeRotationUnit.Slerp();					
				}
			}
		}
		RelativeOrientation = StrafeRelativeOrientation*LookRelativeOrientation;
}

bool Player::Killed()
{
	IDestructable *destr = GetDestructable();
	int lifes = destr->GetLifesNumber();
	if (lifes>1) 
	{
		destr->SetHealth(destr->GetMaxHealth());
		destr->SetLifesNumber(lifes-1);		
	}
	return false;
}

Ogre::Quaternion Player::GetOrientation()
{	
	return SceneObject::GetOrientation() * RelativeOrientation.Inverse();	
}

void Player::SetOrientation(Ogre::Quaternion direction)
{
	SceneObject::SetOrientation(direction * RelativeOrientation);
}

Ogre::Vector3 Player::GetDirection()
{
	return SceneObject::GetOrientation() * RelativeOrientation.Inverse() * Ogre::Vector3::NEGATIVE_UNIT_Z;
}

//void Player::RemoveObject(AAObject *object)
//{
//	TransparentObjects.remove((SceneObject*)object);
//}

void Player::SaveTo(TiXmlElement * root) const
{
	TiXmlElement *elem = new TiXmlElement( "player" ), *elem1 = NULL;  
	root->LinkEndChild( elem );  
	
	SceneObject::SaveTo(elem);
}

void Player::SetActive(bool on)
{
	if (false==on)
	{
		GetEquipped()->SetShooting(false);

		IPhysical *object = GetPhysical();

		//object->SetVelocityVector(Ogre::Vector3::ZERO);
		//object->SetLinVelocity(Ogre::Vector3::ZERO);
		//object->SetThrottle(Ogre::Vector3::ZERO);
		//object->SetLinVelocityAccel(Ogre::Vector3::ZERO);

		object->Stop();

		object->SetRotVelocity(Ogre::Vector3::ZERO);
	}	
}

void Player::SetAutoPilotActive(bool on)
{
	IAI *ai = GetAI();
	
	AutoPilotActive = on;
	ai->SetActive(on);
	/*AIAutoPilotStrategy *auto_pilot = ai->GetAutoPilotStrategy();
	if (auto_pilot)
	{
		auto_pilot->SetActive(on);		
	}*/
}

bool Player::GetAutoPilotActive() const
{
	return AutoPilotActive;
}

void Player::Register()
{
	SceneObject::Register();
	SetAutoPilotActive(false);
}

//bool Player::InitEntity()
//{
//    bool res = SceneObject::InitEntity();
//    ((Ogre::Entity*)OgreEntity)->setNormaliseNormals(true);
//    return res;
//}

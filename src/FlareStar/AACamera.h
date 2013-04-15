#pragma once
#include <Ogre.h>
#define OIS_DYNAMIC_LIB
#include <OIS.h>
#include "InterpolatedRotation.h"
#include "AAObject.h"

//class AAFrameListener;
class Player;
class Room;
class Roomable;
class IMasterEffect;

class AACamera : public AAObject
{
public:
	AACamera(Ogre::Camera *camera);
	~AACamera(void);

	Ogre::Camera *GetOgreCamera();
	void SetNode(Ogre::SceneNode *node);
    //void SetFrameListener(AAFrameListener *listener);
    void Update(unsigned timeMs);
	void Reset();
    void SetInitialOrientation();
    void SetPlayersObject(Player *obj);
    void Frame(unsigned timeMs);
    void ProcessMouseInput(const OIS::MouseState &ms);
    void ProcessKeyInput(OIS::Keyboard *mKeyboard);
    void HandleCameraCollision();

	InterpolatedRotation CameraRotation, CameraAdditionalRotation;

	//void SetParentRoom(Room* room);
	//Room* GetParentRoom();
	Ogre::Vector3 GetPosition() const;

	virtual bool Init();
	virtual void Step(unsigned timeMs);
	virtual IRoomable *GetRoomable() const;

	virtual void Register();
	virtual void Unregister();
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;

	Ogre::SceneNode *GetNode();

	void UnlinkFromPlayer();

	void SetFogMirror(int lid);
	void RemoveFogMirror();

	void SetRotationDamping(float damping);
    void SetSens(float sens);

	void AutoPilotActive( bool on );

    Ogre::Vector3 GetRotVelocity() const;
    
	
protected:
    bool Parse(TiXmlElement* xml);
    void SetPosition(Ogre::Vector3 pos);
	void UpdatePosition();
	Ogre::Camera *OgreCamera;
	Ogre::SceneNode *Node, *CameraTransformationNode;
	IMasterEffect *FogMirror[2];
    //AAFrameListener *Listener;

    Ogre::Quaternion InitialOrientation;
    Ogre::Quaternion RelativeOrientation;
    Player *PlayersObject;
    Ogre::Radian mRotX, mRotY;
    Ogre::Vector3 Position, DefaultPosition, MinPosition, MaxPosition;
	//Room *ParentRoom;

	std::auto_ptr<Roomable> RoomableModule;
	Ogre::AxisAlignedBox Box;

    //unsigned LastTimes[1];
    Ogre::Vector3 AdditionalRelativePosition;
	
	float RotDamping, Sens;
	Ogre::Vector3 RotVelocity;		// rotational velocity	
	Ogre::Vector3 RotAcceleration;	// rotational acceleration
	//Ogre::Matrix3 InvInertia, BoxInertia;		// orientated inverse inertia
	//Ogre::Matrix3 Orientation;	// orientation of the body

	Ogre::TextureUnitState* SkyTexture;
	float TextureScrollX, TextureScrollY;
	Ogre::Quaternion CurrentOrientationForTexture;

	InterpolatedRotation CameraToPlayerRotation, CameraAverageOrientation;
	bool IsAutoPilotActive;

    float APModeStabilizeFps, CamToPlayerFps;
    float RelativeShiftCoeffGrow, RelativeShiftCoeffDump;
    int RelativeShiftMax, RelativeShiftSlowerBuffer;

	Ogre::Quaternion PrevCameraOrientation;
};

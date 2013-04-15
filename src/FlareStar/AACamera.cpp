#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AACamera.h"
#include "Player.h"
#include "CommonDeclarations.h"
#include "Debugging.h"
#include "CollisionDetection.h"
#include "Room.h"
#include "RoomsManager.h"
#include "Collidable.h"
#include "Roomable.h"
#include "IMasterEffect.h"
#include "ObjectsLibrary.h"

AACamera::AACamera(Ogre::Camera *camera):
OgreCamera(camera),
Node(NULL),
CameraTransformationNode(NULL),
RelativeOrientation(Ogre::Quaternion::IDENTITY),
InitialOrientation(Ogre::Quaternion::IDENTITY),
mRotX(0),
mRotY(0),
DefaultPosition(0,150,600),
MinPosition(0,150,300),
MaxPosition(0,150,900),
Position(0,150,600),
AdditionalRelativePosition(Ogre::Vector3::ZERO),
RotDamping(0.89f),
RotVelocity(Ogre::Vector3::ZERO),
RotAcceleration(Ogre::Vector3::ZERO),
Sens(1.f),
SkyTexture(NULL),
TextureScrollX(0),
TextureScrollY(0),
CurrentOrientationForTexture(Ogre::Quaternion::IDENTITY),
PlayersObject(NULL),
IsAutoPilotActive(false),
APModeStabilizeFps(60.f),
CamToPlayerFps(5.f),
RelativeShiftCoeffGrow(0.9f), 
RelativeShiftCoeffDump(0.99f),
RelativeShiftMax(80),
PrevCameraOrientation(Ogre::Quaternion::IDENTITY),
RelativeShiftSlowerBuffer(0)
{
	Box.setMinimum(-10,-10,-10);
	Box.setMaximum(10,10,10);	
	Tag = T_CAMERA;
	memset(FogMirror,0,sizeof(FogMirror));
}

AACamera::~AACamera(void)
{
	Reset();
}

Ogre::Camera *AACamera::GetOgreCamera()
{
	return OgreCamera;
}

void AACamera::SetNode(Ogre::SceneNode *node)
{
    CameraTransformationNode = node;
    //Node = CommonDeclarations::GetSceneManager()->getRootSceneNode()->createChildSceneNode("PlayersCamera");
	Node = node->createChildSceneNode("PlayersCamera");
	Node->setInheritOrientation(false);
	
	OgreCamera->setFixedYawAxis(false);
	
	Node->attachObject(OgreCamera);
		
	Position = DefaultPosition;
	//Node->setPosition(DefaultPosition);
}

void AACamera::SetInitialOrientation()
{
    InitialOrientation = OgreCamera->getOrientation().Inverse();
}

void AACamera::SetPlayersObject(Player *obj) 
{
    PlayersObject=obj;
}

void AACamera::AutoPilotActive( bool on )
{
	IsAutoPilotActive = on;
	if (!IsAutoPilotActive)	
	{
		Ogre::Quaternion common_orientation = PlayersObject->GetOrientation();
		Node->setOrientation(common_orientation);
		CameraTransformationNode->setOrientation(common_orientation);
		//OgreCamera->setFixedYawAxis(false);
	}
}

void AACamera::Update(unsigned timeMs)
{   
	if (!Active)
		return;
	
	float dt = timeMs / 1000.f;
	
	RotAcceleration.x = mRotX.valueDegrees()*Sens;
	RotAcceleration.y = mRotY.valueDegrees()*Sens;
	RotVelocity += RotAcceleration * dt;
	
	RotVelocity *= RotDamping;
	
	Ogre::Quaternion q;
	Ogre::Quaternion new_cam_orientation = Node->getWorldOrientation();
	Ogre::Quaternion new_player_orientation = PlayersObject->GetOrientation();
	//Ogre::Quaternion new_player_orientation = new_cam_orientation;

	Ogre::Vector3 cam_y = new_player_orientation * Ogre::Vector3::UNIT_Y;
	Ogre::Vector3 cam_x = new_cam_orientation * Ogre::Vector3::UNIT_X;

	//if (IsAutoPilotActive)
	//{		
	//	cam_y = Ogre::Vector3::UNIT_Y;
	//} else
	//{
	//	cam_y = new_player_orientation * Ogre::Vector3::UNIT_Y;
	//}
			
    if (IsAutoPilotActive)
    {
		//cam_y = Ogre::Vector3::UNIT_Y;

		q = Ogre::Quaternion::IDENTITY;
		
		q.FromAngleAxis(Ogre::Degree(RotVelocity.x),cam_y);
		new_cam_orientation = q * new_cam_orientation;

		//OgreCamera->pitch(Ogre::Degree(RotVelocity.y));

		q = Ogre::Quaternion::IDENTITY;
		q.FromAngleAxis(Ogre::Degree(RotVelocity.y),cam_x);
		new_cam_orientation = q * new_cam_orientation;

		Ogre::Quaternion InverseAdditional = RelativeOrientation.Inverse();
		Ogre::Quaternion CameraOrientation = new_cam_orientation*InverseAdditional;    
		Ogre::Vector3 lookdir(CameraOrientation*Ogre::Vector3::NEGATIVE_UNIT_Z);
				
		Ogre::Vector3 xVec = cam_y.crossProduct(lookdir);
		xVec.normalise();
		Ogre::Vector3 yVec = lookdir.crossProduct(xVec);
		yVec.normalise();
		Ogre::Quaternion unitZToTarget = Ogre::Quaternion(xVec, yVec, lookdir);

		Ogre::Quaternion targetOrientation = Ogre::Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

		if (CameraOrientation!=targetOrientation)
			CameraRotation.StartRotation(CameraOrientation, targetOrientation, APModeStabilizeFps);

		//

		if(CameraRotation.mRotating)
		{
			CameraRotation.Step();
			if(CameraRotation.mRotating)
			{
				new_cam_orientation = CameraRotation.Slerp();
			}
		}

		//

		CameraTransformationNode->setOrientation(new_cam_orientation);		
	} else
	{	
        //cam_y = new_player_orientation * Ogre::Vector3::UNIT_Y;

		//OgreCamera->yaw(Ogre::Degree(RotVelocity.x));

		q.FromAngleAxis(Ogre::Degree(RotVelocity.x),cam_y);
		new_player_orientation = q * new_player_orientation;

		//OgreCamera->pitch(Ogre::Degree(RotVelocity.y));

		q = Ogre::Quaternion::IDENTITY;

		q.FromAngleAxis(Ogre::Degree(RotVelocity.y),cam_x);
		new_player_orientation = q * new_player_orientation;

		{
			Ogre::Vector3 lookdir(new_player_orientation*Ogre::Vector3::NEGATIVE_UNIT_Z);

			Ogre::Vector3 xVec = cam_y.crossProduct(lookdir);
			xVec.normalise();
			Ogre::Vector3 yVec = lookdir.crossProduct(xVec);
			yVec.normalise();
			Ogre::Quaternion unitZToTarget = Ogre::Quaternion(xVec, yVec, lookdir);

			new_player_orientation = Ogre::Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);
		}
				
		PlayersObject->SetOrientation(new_player_orientation);

		Ogre::Quaternion curr = CameraTransformationNode->getWorldOrientation();
		CameraToPlayerRotation.StartRotation(curr, new_player_orientation, CamToPlayerFps);		

		if(CameraToPlayerRotation.mRotating)
		{
			CameraToPlayerRotation.Step();
			if(CameraToPlayerRotation.mRotating)
			{                
				Ogre::Quaternion delta = CameraToPlayerRotation.Slerp();
				CameraTransformationNode->setOrientation(delta);
				//new_cam_orientation = delta;

                CameraAverageOrientation.StartRotation(delta, new_player_orientation, 1);
                CameraAverageOrientation.mRotProgress = 0.5;
                new_cam_orientation = CameraAverageOrientation.Slerp();
			}
		}
	}

	
	
	Node->setOrientation(new_cam_orientation);
	

	if (SkyTexture)
	{
		Ogre::Vector3 v1(new_cam_orientation*Ogre::Vector3::NEGATIVE_UNIT_Z), v2(PrevCameraOrientation*Ogre::Vector3::NEGATIVE_UNIT_Z), v3;
		PrevCameraOrientation = new_cam_orientation;
		
		Ogre::Quaternion q_roll, q_diff = v2.getRotationTo(v1);

		q_roll.FromAngleAxis(new_cam_orientation.getRoll(), Ogre::Vector3::UNIT_Z);

		//v3.x = q_diff.getYaw().valueDegrees()/360.f;
		//v3.y = q_diff.getPitch().valueDegrees()/360.f;

		v3 = q_diff*q_roll*Ogre::Vector3::UNIT_Z;
		
		//v3 = v2 - v1;

		//v3.x=RotVelocity.x*3/360.f;
		//v3.y=RotVelocity.y*3/360.f;

		TextureScrollX+=v3.x;
		TextureScrollY+=v3.y;

		char log[100];
		sprintf(log,"1 %f %f :: %f %f\n", v3.x, v3.y, TextureScrollX, TextureScrollY);
		Debugging::Log("cam",log);
		
		/*Ogre::Radian yaw = new_cam_orientation.getYaw(), pitch = new_cam_orientation.getPitch();
		TextureScrollX=pitch.valueDegrees()/180.f;
		TextureScrollY=yaw.valueDegrees()/180.f;*/
		
		/*TextureScrollX+=RotVelocity.x*3/360.f;
		TextureScrollY-=RotVelocity.y*3/360.f;*/
		
		Ogre::Vector3 scroll(TextureScrollX, TextureScrollY, 0);
		
		SkyTexture->setTextureScroll(scroll.x, scroll.y);
	}
}

void AACamera::Frame(unsigned timeMs)
{
	//Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
    /*if(CameraAdditionalRotation.mRotating)
    {
        CameraAdditionalRotation.Step();
        if(CameraAdditionalRotation.mRotating)
        {                
            RelativeOrientation = CameraAdditionalRotation.Slerp();                
			q = RelativeOrientation;
        }
    }*/
   
    Ogre::Vector3 accel = -PlayersObject->GetPhysical()->GetThrottle();
    accel.normalise();
	
	/*char log[100];
	sprintf(log,"1 %f %f %f\n", accel.x, accel.y, accel.z);
	Debugging::Log("cam",log);*/

    bool accel_is_zero = accel.isZeroLength();
	if (!accel_is_zero)
	{
		if (accel.x==0) {
			AdditionalRelativePosition.x*=RelativeShiftCoeffDump;
		}
		if (accel.y==0) {
			AdditionalRelativePosition.y*=RelativeShiftCoeffDump;
		}
		if (accel.z==0) {
			AdditionalRelativePosition.z*=RelativeShiftCoeffDump;
		}

		Ogre::Vector3 shift_size;
		
		shift_size.x = abs(AdditionalRelativePosition.x);
		shift_size.y = abs(AdditionalRelativePosition.y);
		shift_size.z = abs(AdditionalRelativePosition.z);
		
		if (shift_size.x>=RelativeShiftMax) {
			accel.x=0;
		} else
			if (shift_size.x>RelativeShiftMax-RelativeShiftSlowerBuffer)
			{
				accel.x/=2;
			}

		
		if (abs(AdditionalRelativePosition.y)>=RelativeShiftMax) {
			accel.y=0;
		} else
			if (shift_size.y>RelativeShiftMax-RelativeShiftSlowerBuffer)
			{
				accel.y/=2;
			}

		if (abs(AdditionalRelativePosition.z)>=RelativeShiftMax) {
			accel.z=0;
		} else
			if (shift_size.z>RelativeShiftMax-RelativeShiftSlowerBuffer)
			{
				accel.z/=2;
			}

		if (!accel.isZeroLength())
			AdditionalRelativePosition+=accel*RelativeShiftCoeffGrow;
	}
	else
		if (accel_is_zero && !AdditionalRelativePosition.isZeroLength())
			AdditionalRelativePosition*=RelativeShiftCoeffDump;
	
	//Node->setPosition(Position+AdditionalRelativePosition);
	
    HandleCameraCollision();

	UpdatePosition();
}

void AACamera::SetPosition(Ogre::Vector3 pos)
{
    Position = pos;
    //Node->setPosition(CameraTransformationNode->getWorldPosition() + pos + AdditionalRelativePosition);
}

void AACamera::UpdatePosition()
{
	//Node->setPosition(CameraTransformationNode->getWorldPosition() + CameraTransformationNode->getWorldOrientation()*(Position + AdditionalRelativePosition) );
	Node->setPosition(Position + AdditionalRelativePosition);
}

void AACamera::ProcessMouseInput(const OIS::MouseState &ms)
{
    using namespace OIS;

    // Rotation factors, may not be used if the second mouse button is pressed
    // 2nd mouse button - slide, otherwise rotate
    //const OIS::MouseState &ms = mMouse->getMouseState();
    //if( ms.buttonDown( MB_Right ) )
    //{
    //    //mTranslateVector.x += ms.X.rel * 0.13;
    //    //mTranslateVector.y -= ms.Y.rel * 0.13;		
    //}
    //else
    {				
        mRotX = Ogre::Degree(-ms.X.rel * 0.13);				
        mRotY = Ogre::Degree(-ms.Y.rel * 0.13);
    }

    if (ms.Z.rel!=0) 
    {
        Ogre::Real shift = ms.Z.rel*0.13;
        if ((Position.z-shift>=MinPosition.z) && (Position.z-shift<MaxPosition.z))
        {
            Position.z-=shift;
            //Node->setPosition(Position+AdditionalRelativePosition);
			//UpdatePosition();
        }
    }        
}

void AACamera::ProcessKeyInput(OIS::Keyboard *mKeyboard)
{
    static unsigned RollSpeed = 5;
    if (mKeyboard->isKeyDown(OIS::KC_RIGHT))
    {		
        if (abs(RelativeOrientation.getRoll().valueDegrees())<10)
        {
            Ogre::Quaternion q;
            q.FromAngleAxis(-Ogre::Degree(RollSpeed),Ogre::Vector3::UNIT_Z);
            CameraAdditionalRotation.StartRotation(RelativeOrientation, RelativeOrientation*q, CommonDeclarations::GetFPS());			
        }		
    }
    else
        if(mKeyboard->isKeyDown(OIS::KC_LEFT))
        {
            if (abs(RelativeOrientation.getRoll().valueDegrees())<10)
            {
                Ogre::Quaternion q;
                q.FromAngleAxis(Ogre::Degree(RollSpeed),Ogre::Vector3::UNIT_Z);
                CameraAdditionalRotation.StartRotation(RelativeOrientation, RelativeOrientation*q, CommonDeclarations::GetFPS());				
            }			
        }
        else
        {
            if (RelativeOrientation!=Ogre::Quaternion::IDENTITY)			
            {
                Ogre::Quaternion q = Ogre::Quaternion::IDENTITY;
                CameraAdditionalRotation.StartRotation(RelativeOrientation, q, CommonDeclarations::GetFPS());				
            }
        }
}

void AACamera::HandleCameraCollision()
{	
    static bool HandlingCollision=false;
    Ogre::Vector3 origin = PlayersObject->GetPosition(), camabspos;
    
    if (HandlingCollision)
    {
        Ogre::Matrix4 matr;
        CameraTransformationNode->getWorldTransforms(&matr);
        camabspos = matr * DefaultPosition;
    }
    else
        camabspos=Node->getWorldPosition();

    Ogre::Vector3 direction = camabspos - origin;	
    Ogre::Ray ray(origin,direction);
    
    Room *room=NULL;

	IRoomable::RoomsPool *pool = ((IRoomable*)RoomableModule.get())->GetRooms();

	bool collided=false;

	if (NULL!=pool && !pool->IsEmpty())
	{   
		Room::ObjectType *object=NULL;

		Room::ObjectsPool *objects_pool = NULL;
		//Room::ObjectsPool::iterator objects_iPos, objects_iEnd;
		Room::ObjectsPool::ListNode *objpos=NULL;
		ICollidable *collid = NULL;
		IScenable *scen = NULL;
		Ogre::Vector3 pos;

		objects_pool = pool->GetBegin()->Value->GetChildObjects();

		for (objpos = objects_pool->GetBegin();objpos;objpos=objpos->Next)
		{
			object = objpos->Value;
			if (object == this)
				continue;
			collid = object->GetCollidable();
			scen = object->GetScenable();

			if (collid && scen && object!=PlayersObject && scen->GetType()==PT_STATIC&& collid && collid->Collide(ray))
			{
				Ogre::Vector3 colpos = object->GetCollidable()->GetCollisionDetection()->GetCollisionPoint();
				colpos = colpos - origin;
				int shift=0;

				if (colpos.z-shift>=MinPosition.z)
				{
				    Position.z=colpos.z-shift;
				    //Node->setPosition(Position);
					//UpdatePosition();
				    HandlingCollision=true;

				    if (Position.z<2*PlayersObject->GetPhysical()->GetRadius() && PlayersObject->GetTransparencyPercent()==0)
				        PlayersObject->SetTransparency(0.5);
				}                
				
				collided=true;
				break;
			}
		}            	

	}
 
    if (!collided)
    {
        if (HandlingCollision)
        {
            Position = DefaultPosition;
			//UpdatePosition();
            HandlingCollision = false;
        }
        
        if (PlayersObject->GetTransparencyPercent()>0 && Position.z>2*PlayersObject->GetPhysical()->GetRadius())
            PlayersObject->SetTransparency(0);
    }
}

Ogre::Vector3 AACamera::GetPosition() const
{
	return Position;
}

bool AACamera::Init()
{
	Active = true;
	RoomableModule.reset(new Roomable(this));
	RoomableModule->SetRoomOnly(IRoomable::ROM_NONE);
	RoomableModule->SetBox(GetBoundingBox(true));	
	CommonDeclarations::GetRoomManager()->PutObject(this);

	/*Ogre::Degree fov(60);
	const Ogre::Radian fov_r(fov.valueRadians()) ;
	OgreCamera->setFOVy(fov_r);*/

    TiXmlElement *game_config = CommonDeclarations::GetGameConfig();
    assert(game_config);
    if (game_config)
        Parse(game_config);

	return true;
}

void AACamera::Step(unsigned timeMs)
{
    Update(timeMs);
	RoomableModule->SetBox(GetBoundingBox(true));
	CommonDeclarations::GetRoomManager()->UpdateObject(this);
	Frame(timeMs);
}

IRoomable *AACamera::GetRoomable() const
{
	return RoomableModule.get();
}

void AACamera::Register()
{	
	Init();

	AAObject::Register();
}

void AACamera::Unregister()
{
	AAObject::Unregister();
	if (RoomableModule.get())
	{
		RoomableModule->RemoveFromRooms();
		RoomableModule->GetRooms()->Clear();			
	}
}

Ogre::AxisAlignedBox AACamera::GetBoundingBox(bool transformed) const
{	
	Ogre::AxisAlignedBox box = Box;
	if (!transformed)
		return box;

	Ogre::Matrix4 transforms;
	//Node->getWorldTransforms(&transforms);
	transforms.makeTransform(Node->getPosition(),Node->getScale(),Node->getOrientation());
	box.transform(transforms);
	return box;
}

Ogre::SceneNode *AACamera::GetNode()
{
	//return CameraTransformationNode;
	return Node;
}

void AACamera::UnlinkFromPlayer()
{
//	Node->detachAllObjects();

	PlayersObject = NULL;
	CameraTransformationNode = NULL;
}

void AACamera::SetFogMirror(int lid)
{
	//assert(NULL==FogMirror);

	IMasterEffect *effect = ObjectsLibrary::GetInstance()->LoadEffect(lid);    
	assert(effect);

	if (effect) 
	{		
		for (size_t i = 0;i<2;++i)
		{
			if (NULL==FogMirror[i])
			{
				effect->SetParentNode(CameraTransformationNode); //CameraTransformationNode        //CameraTransformationNode->createChildSceneNode(effect->GetName())
				effect->Init();				
				//effect->GetNode()->setInheritOrientation(false);
				effect->Start();
				FogMirror[i] = effect;

				Ogre::MaterialPtr skyMat = Ogre::MaterialManager::getSingleton().getByName("sky_sphere_material");
				
				// Perform no dynamic lighting on the sky
				//skyMat->setLightingEnabled(false);				
				SkyTexture = skyMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
				

				break;
			}
		}
	}
}

void AACamera::RemoveFogMirror()
{	
	for (size_t i = 0;i<2;++i)
	{
		if (FogMirror[i])
			FogMirror[i]->Stop();
	}	
}

void AACamera::Reset()
{
	for (size_t i = 0;i<2;++i)
	{
		if (FogMirror[i])
		{
			FogMirror[i]->Destroy();
			FogMirror[i] = NULL;
		}
	}
}

void AACamera::SetRotationDamping(float damping)
{
	RotDamping = damping;
}

void AACamera::SetSens(float sens)
{
    Sens = sens;
}

bool AACamera::Parse(TiXmlElement* xml)
{   
    if (xml == 0)
        return false; // file could not be opened

    TiXmlElement *xml_element=NULL, *xml_element1=NULL;
    //const char *str;
    
    xml_element = xml;

    int val, res;
   
    xml_element = xml->FirstChildElement("camera");
    assert(xml_element);

    if (xml_element)
    {
        res = xml_element->QueryFloatAttribute("ap_stabilize",&APModeStabilizeFps);
        assert(TIXML_SUCCESS==res);

        res = xml_element->QueryFloatAttribute("cam2player",&CamToPlayerFps);
        assert(TIXML_SUCCESS==res);

        res = xml_element->QueryFloatAttribute("rs_grow",&RelativeShiftCoeffGrow);
        assert(TIXML_SUCCESS==res);

        res = xml_element->QueryFloatAttribute("rs_dump",&RelativeShiftCoeffDump);
        assert(TIXML_SUCCESS==res);

        res = xml_element->QueryIntAttribute("rs_max",&RelativeShiftMax);
        assert(TIXML_SUCCESS==res);

		res = xml_element->QueryIntAttribute("rs_buffer",&RelativeShiftSlowerBuffer);
		assert(TIXML_SUCCESS==res);

        res = xml_element->QueryIntAttribute("fov",&val);
        assert(TIXML_SUCCESS==res);

        Ogre::Degree fov(val);
        const Ogre::Radian fov_r(fov.valueRadians()) ;
        OgreCamera->setFOVy(fov_r);

        int x,y,z;
        xml_element1 = xml_element->FirstChildElement("min");
        if (xml_element1)
        {
            x=y=z=0;
            res = xml_element1->QueryIntAttribute("x", &x);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("y", &y);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("z", &z);
            assert(TIXML_SUCCESS==res);
            MinPosition = Ogre::Vector3(x, y, z);
        }

        xml_element1 = xml_element->FirstChildElement("max");
        if (xml_element1)
        {
            x=y=z=0;
            res = xml_element1->QueryIntAttribute("x", &x);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("y", &y);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("z", &z);
            assert(TIXML_SUCCESS==res);
            MaxPosition = Ogre::Vector3(x, y, z);
        }

        xml_element1 = xml_element->FirstChildElement("default");
        if (xml_element1)
        {
            x=y=z=0;
            res = xml_element1->QueryIntAttribute("x", &x);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("y", &y);
            assert(TIXML_SUCCESS==res);
            res = xml_element1->QueryIntAttribute("z", &z);
            assert(TIXML_SUCCESS==res);
            DefaultPosition = Ogre::Vector3(x, y, z);
        }
    }    

    return true;
}

Ogre::Vector3 AACamera::GetRotVelocity() const
{
    return RotVelocity;
}


#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Physical.h"
#include "Debugging.h"
#include "Room.h"

#include "SceneObject.h"

#include "CommonDeclarations.h"

#include "coldet.h"
#include "Debugging.h"


#include "MeshCollisionDetection.h"
#include "SphereCollisionDetection.h"
#include "MeshAndSphereCollisionDetection.h"

#include "TriKickCollisionResponse.h"
#include "PivotKickCollisionResponse.h"
#include "SlideCollisionResponse.h"

#include "EquippedObject.h"

#include "AIModule.h"
#include "Utilities.h"
#include "Collidable.h"
#include "IRoomable.h"

#define MAX_VELOCITY		60
#define VELOCITY_STEP 20.f

Physical::Physical(void) :
AccelerationOn(0,0,0),
//MaxVelocity(MAX_VELOCITY),
Mass(30),
Position(Ogre::Vector3::ZERO),
PrevPosition(Ogre::Vector3::ZERO),
Radius(0),
MaxVelocities(60,60,60),
VelocitiesStep(20,20,20)
{
	Descriptor.Object = this;
}

Physical::~Physical(void)
{	
}

double Physical::GetRadius() const
{
	return Radius;
}

void	Physical::InitPhysics()
{	
	Ogre::AxisAlignedBox box = Owner->GetScenable()->GetBoundingBox(false);
		
	//Owner->GetNode()->setPosition(Position);
	Size		= box.getSize();
	Radius		= Size.length()/2;
	
	Ogre::Real x2 = (Size.x * Size.x);
	Ogre::Real y2 = (Size.y * Size.y);
	Ogre::Real z2 = (Size.z * Size.z);
	Ogre::Real ix = (y2 + z2) * Mass / 12.0f;
	Ogre::Real iy = (x2 + z2) * Mass / 12.0f;
	Ogre::Real iz = (x2 + y2) * Mass / 12.0f;

	BoxInertia = BoxInertia.IDENTITY;// makeIdentity();

	BoxInertia[0][0] = ix;
	BoxInertia[1][1] = iy;
	BoxInertia[2][2] = iz;
	
	
	Descriptor.LinVelocity= Ogre::Vector3::ZERO;
	Descriptor.RotVelocity= Ogre::Vector3::ZERO;
	Descriptor.LinAcceleration= Ogre::Vector3::ZERO;
	Descriptor.RotAcceleration= Ogre::Vector3::ZERO;
	Forces= Ogre::Vector3::ZERO;
	Torques= Ogre::Vector3::ZERO;

	LinDamping = RotDamping = 0.99f;

	Owner->GetScenable()->GetAbsoluteOrientation().ToRotationMatrix(Orientation);
	InvInertia = Orientation * BoxInertia.Inverse() * Orientation.Inverse();
}

Physical::WaypointsPool *Physical::GetWaypoints()
{
	return &Waypoints;
}

Ogre::Vector3 Physical::GetPosition() const
{
    return Position;
}

Ogre::Vector3 Physical::GetPrevPosition() const
{
	return PrevPosition;
}

void Physical::SetPosition(Ogre::Vector3 pos)
{
    Position = pos;
}

Vector3 Physical::GetLinVelocity() const
{
	return Descriptor.LinVelocity;
}

void Physical::Update(Ogre::Real dt)
{
	/*char log[100];
	sprintf(log,"1 %f\n", dt);
	Debugging::Log("up",log);*/
	AddForce(Position, Descriptor.VelocityVector);

	//-------------------------------------------------------
	// Euler Integration
	//-------------------------------------------------------
	Descriptor.LinAcceleration  = Forces / Mass;
	Descriptor.RotAcceleration= Torques*InvInertia;

	PrevPosition = Position;
	Descriptor.LastVelocity = Descriptor.LinVelocity * dt + Descriptor.LinAcceleration * (dt*dt*0.5f);
	Position += Descriptor.LastVelocity; //Descriptor.LinVelocity * dt + Descriptor.LinAcceleration * (dt*dt*0.5f);
	
	Descriptor.RotVelocity += Descriptor.RotAcceleration * dt;

	const float max_rot_velocity = 2.f, sq_max_rot_velocity = max_rot_velocity*max_rot_velocity;
	if (Descriptor.RotVelocity.x*Descriptor.RotVelocity.x>sq_max_rot_velocity)
	{
		Descriptor.RotVelocity.x = max_rot_velocity;
	}
	if (Descriptor.RotVelocity.y*Descriptor.RotVelocity.y>sq_max_rot_velocity)
	{
		Descriptor.RotVelocity.y = max_rot_velocity;
	}
	if (Descriptor.RotVelocity.z*Descriptor.RotVelocity.z>sq_max_rot_velocity)
	{
		Descriptor.RotVelocity.z = max_rot_velocity;
	}

	
	////if (ID==2)
	//{
		/*char log[100];
		sprintf(log,"1 %d %f %f %f\n",Owner->GetScriptable()->GetID(),Descriptor.LastVelocity.x,Descriptor.LastVelocity.y,Descriptor.LastVelocity.z);	
		sprintf(log,"2 %d %f %f %f)\n", Owner->GetScriptable()->GetID(),Position.x,Position.y,Position.z);
		Debugging::Log("4",log);*/
	//}	
		

	//-----------------------------------------------------------------------
	// Calculate new linear velocity
	//-----------------------------------------------------------------------
	//if (Descriptor.Collided)
	//LinVelocity += LinAcceleration * dt;
	//else
	//LinVelocity += Descriptor.VelocityVector * dt;


	Descriptor.LinVelocityBase = Descriptor.VelocityVector * dt;
	Descriptor.LinVelocityAccel += Descriptor.LinAcceleration * dt;
	Descriptor.LinVelocity = Descriptor.LinVelocityBase + Descriptor.LinVelocityAccel;

	//-----------------------------------------------------------------------
	// A bit of damping
	//-----------------------------------------------------------------------
	
	if (LinDamping>0)
		Descriptor.LinVelocityAccel *= LinDamping;
	if (RotDamping>0)
		Descriptor.RotVelocity     *= RotDamping;

	//-----------------------------------------------------------------------
	// Reset forces
	//-----------------------------------------------------------------------
	Descriptor.LinAcceleration	= Ogre::Vector3::ZERO;
	Descriptor.RotAcceleration	= Ogre::Vector3::ZERO;
	Forces			= Ogre::Vector3::ZERO;
	Torques			= Ogre::Vector3::ZERO;
	//Descriptor.VelocityVector = Vector3(0, 0, 0);

	UpdateMatrices();	
}

void Physical::UpdateMatrices()
{
	Owner->GetScenable()->GetAbsoluteOrientation().ToRotationMatrix(Orientation);
	InvInertia = Orientation * BoxInertia.Inverse() * Orientation.Inverse();

	if (!AccelerationOn.isZeroLength())
		SetAcceleration(AccelerationOn);
}

void Physical::AddForce(Vector3 P, Vector3 F)
{
	Forces  += F;
	Torques += (P - Position).crossProduct(F);
}


//----------------------------------------------------------
// Add a collision impulse when the box collide with the floor
// 
//			j =			-(1+Cor)(Vel.norm)
//				 -------------------------------------
//			     1/Mass + norm.(((r x norm) / Inertia) x r)
//
//----------------------------------------------------------
void Physical::AddCollisionImpulse(Vector3 P, Vector3 N, PhysObjDescriptor *dwith)
{
	//------------------------------------------------------------------------
	// calculate the velocity at contact point.
	//------------------------------------------------------------------------
	Vector3 R = P - Position;

	//char log[100];

	Vector3 vel=Descriptor.LinVelocity;
	
	if (dwith && vel.squaredLength()<0.1*0.1)
	{		
		vel = -dwith->LinVelocity/Mass;
		//sprintf(log,"A %d %f %f %f\n", ID, vel.x, vel.y, vel.z);
		//Debugging::Log("ph",log);		
	}
	
	//vel = Vector3(0,0,-20);
	
	
	Vector3 V = vel + (Descriptor.RotVelocity.crossProduct(R));

	Ogre::Real numer = -(1 + 0.3f) * (N.dotProduct(V)); // 0.3 Coefficient of Restitution

	Vector3 p(R.crossProduct(N)), v(p*InvInertia); //v(p.x * InvInertia[0][0], p.y * InvInertia[1][1], p.z * InvInertia[2][2]);
	Ogre::Real denom = 1/Mass + (v.crossProduct(R)).dotProduct(N);

	Ogre::Real Impulse = numer / denom;

	if (Impulse < 0.0f)
		return;

	Ogre::Real dt = 1.0f/TARGET_UPDATE_INTERVAL_MS;

	Ogre::Real force = Impulse / dt;

	AddForce(P, N * force);	
}

void Physical::SetReplacingDirection(Ogre::Vector3 direction)
{
	Descriptor.ReplacingDirection=direction;
}

void Physical::CalcVelocityVector(unsigned diff)
{
	Vector3 xAxis, yAxis, zAxis;

	Descriptor.VelocityVector= Vector3::ZERO;

	//ICollidable *collid = Owner->GetCollidable();

	if (/* collid && collid->GetCollising() && */!Descriptor.ReplacingDirection.isZeroLength())
	{
		Descriptor.VelocityVector += Descriptor.ReplacingDirection*(Ogre::Real)diff;
		Descriptor.LinVelocityAccel = Descriptor.ReplacingDirection*(Ogre::Real)diff;

		//char log[100];
		//sprintf(log,"replace %f %f %f\n",Descriptor.VelocityVector.x,Descriptor.VelocityVector.y,Descriptor.VelocityVector.z);			
		//Debugging::Log("4",log);

		//Descriptor.ReplacingDirection=Ogre::Vector3::ZERO;
		//char log[100];
		//sprintf(log,"1 %d %f %f %f\n",ID,LinVelocityAccel.x,LinVelocityAccel.y,LinVelocityAccel.z);
		//sprintf(log,"1\n");
		//Debugging::Log("sl",log);
		//Descriptor.Collising=false;
	} else	
	{
		//Descriptor.ReplacingDirection=Ogre::Vector3::ZERO;
		
		Quaternion orientation = Owner->GetScenable()->GetOrientation();

		xAxis= orientation*Vector3::UNIT_X;
		yAxis= orientation*Vector3::UNIT_Y;
		zAxis= Descriptor.ForwardDirection;

		Descriptor.VelocityVector += xAxis*Descriptor.Throttle.x*(Ogre::Real)diff;
		Descriptor.VelocityVector += yAxis*Descriptor.Throttle.y*(Ogre::Real)diff;	
		Descriptor.VelocityVector += zAxis*Descriptor.Throttle.z*(Ogre::Real)diff;

		//char log[100];
		//sprintf(log,"normal %f %f %f\n",Descriptor.VelocityVector.x,Descriptor.VelocityVector.y,Descriptor.VelocityVector.z);			
		//Debugging::Log("4",log);

		//char log[100];
		//sprintf(log,"1 %d %f %f %f\n",ID,LinVelocityAccel.x,LinVelocityAccel.y,LinVelocityAccel.z);
		//sprintf(log,"2\n");
		//Debugging::Log("sl",log);
	}
	Descriptor.ReplacingDirection=Ogre::Vector3::ZERO;
}

bool Physical::Parse(TiXmlElement* xml)
{	
	if (xml == 0)
		return false; // file could not be opened
	
	const char *str;
	
	//str = xml->Value();

	//assert(strcmp(str,"obj")==0 || strcmp(str,"node")==0 || strcmp(str,"player")==0 || strcmp(str,"ammo")==0);
		
	str = xml->Attribute("cfg");

	if (str)
	{
		return XMLConfigurable::Parse(str);
	}
	
	//xml->QueryIntAttribute("maxspeed",&MaxVelocity);
    	
    //TiXmlElement *xml_element = 0;

    str = xml->Attribute("collisresponse");

    if (str) 
    {
        if (strcmp(str,"slide")==0)
            CollisionResponseModule.reset(new SlideCollisionResponse(this));
        else
            if (strcmp(str,"trikick")==0)
                CollisionResponseModule.reset(new TriKickCollisionResponse(this));
            else
                if (strcmp(str,"pivotkick")==0)
                    CollisionResponseModule.reset(new PivotKickCollisionResponse(this));
                else
                    assert(false);
    } else
    {
        CollisionResponseModule.reset(new TriKickCollisionResponse(this));
    }

    int res, vx=0,vy=0,vz=0;
    TiXmlElement *xml_element = xml->FirstChildElement("physics");
    assert(xml_element);
    if (xml_element)
    {
        res = xml_element->QueryIntAttribute("maxspeed_x",&vx);
        assert(TIXML_SUCCESS==res);
        res = xml_element->QueryIntAttribute("maxspeed_y",&vy);
        assert(TIXML_SUCCESS==res);
        res = xml_element->QueryIntAttribute("maxspeed_z",&vz);
        assert(TIXML_SUCCESS==res);
        MaxVelocities.x = vx;
        MaxVelocities.y = vy;
        MaxVelocities.z = vz;

        res = xml_element->QueryIntAttribute("stepspeed_x",&vx);
        assert(TIXML_SUCCESS==res);
        res = xml_element->QueryIntAttribute("stepspeed_y",&vy);
        assert(TIXML_SUCCESS==res);
        res = xml_element->QueryIntAttribute("stepspeed_z",&vz);
        assert(TIXML_SUCCESS==res);
        VelocitiesStep.x = vx;
        VelocitiesStep.y = vy;
        VelocitiesStep.z = vz;

        xml_element->QueryDoubleAttribute("mass",&Mass);
        assert(TIXML_SUCCESS==res);
		
		xml_element->QueryFloatAttribute("lin_damping",&LinDamping);
		//assert(TIXML_SUCCESS==res);

		xml_element->QueryFloatAttribute("rot_damping",&RotDamping);
		//assert(TIXML_SUCCESS==res);
    }
	
	return true;
}

Ogre::Vector3 Physical::GetRotVelocity() const
{
	return Descriptor.RotVelocity;
}

void Physical::SetRotVelocity(Ogre::Vector3 vel)
{
	Descriptor.RotVelocity = vel;
}

void Physical::SetThrottle(Vector3 v)
{
    Descriptor.Throttle = v;
}

bool Physical::IsCollided() const
{
    return false;
}

void Physical::SetCollided(bool on)
{
    //Descriptor.Collided = on;
}

void Physical::SetForwardDirection(Ogre::Vector3 direction)
{
	Descriptor.ForwardDirection = direction;
}

Ogre::Vector3 Physical::GetForwardDirection()
{
	return Descriptor.ForwardDirection;
}

void Physical::SetAcceleration(Ogre::Vector3 acceleration)
{
	AccelerationOn = acceleration;
	if (AccelerationOn.x==0)
		Descriptor.Throttle.x=0;
	else
		if (abs(Descriptor.Throttle.x)<=MaxVelocities.x || Descriptor.Throttle.x*AccelerationOn.x<0)
			Descriptor.Throttle.x+=AccelerationOn.x*VelocitiesStep.x;

	if (AccelerationOn.y==0)
		Descriptor.Throttle.y=0;
	else
		if (abs(Descriptor.Throttle.y)<=MaxVelocities.y || Descriptor.Throttle.y*AccelerationOn.y<0)
			Descriptor.Throttle.y+=AccelerationOn.y*VelocitiesStep.y;

	if (AccelerationOn.z==0)
		Descriptor.Throttle.z=0;
	else
		if (abs(Descriptor.Throttle.z)<=MaxVelocities.z || Descriptor.Throttle.z*AccelerationOn.z<0)
			Descriptor.Throttle.z+=AccelerationOn.z*VelocitiesStep.z;

	
	/*
	char log[100];
	sprintf(log,"1 %d %f %f %f\n",ID,Descriptor.Throttle.x, Descriptor.Throttle.y, Descriptor.Throttle.z);
	Debugging::Log("caccel",log);
	sprintf(log,"2 %f %f %f\n",AccelerationOn.x, AccelerationOn.y, AccelerationOn.z);
	Debugging::Log("caccel",log);
	*/
	
}
void Physical::SetLinDamping(float damping)
{
	LinDamping = damping;
}

void Physical::SetRotDamping(float damping)
{
	RotDamping = damping;
}

void Physical::SetForces(Ogre::Vector3 v)
{
	Forces = v;
}

Ogre::Vector3 Physical::GetThrottle() const
{
	return Descriptor.Throttle;
}

double Physical::GetMass() const
{
	return Mass;
}

void Physical::SetOwner(IAAObject* owner)
{
    Owner = owner;
}

IAAObject *Physical::GetOwner()
{
    return Owner;
}

CollisionResponse *Physical::GetCollisionResponse()
{
    return CollisionResponseModule.get();
}

Ogre::Vector3 Physical::GetAccelerationOn() const
{
    return AccelerationOn;
}

//void Physical::SetMaxVelocity(int vel)
//{
//	MaxVelocity = vel;
//}
//
//int Physical::GetMaxVelocity() const
//{
//	return MaxVelocity;
//}

void Physical::SetMaxVelocities(Ogre::Vector3 vel)
{
    MaxVelocities = vel;
}

Ogre::Vector3 Physical::GetMaxVelocities() const
{
    return MaxVelocities;
}

void Physical::Stop()
{
	AccelerationOn = Ogre::Vector3::ZERO;
	Descriptor.VelocityVector = Ogre::Vector3::ZERO;
	//LinVelocity = Ogre::Vector3::ZERO;
	SetForces(Ogre::Vector3::ZERO);

	SetVelocityVector(Ogre::Vector3::ZERO);
	SetLinVelocity(Ogre::Vector3::ZERO);
	SetThrottle(Ogre::Vector3::ZERO);
	SetLinVelocityAccel(Ogre::Vector3::ZERO);
}

void Physical::SetVelocityVector(Ogre::Vector3 vect)
{
	Descriptor.VelocityVector = vect;
}

void Physical::SetLinVelocity(Ogre::Vector3 vect)
{
	Descriptor.LinVelocity = vect;
}

void Physical::SetLinVelocityAccel(Ogre::Vector3 vect)
{
	Descriptor.LinVelocityAccel = vect;
}

void Physical::Step(unsigned timeMs)
{	
	//char log[100];
	//sprintf(log,"1 %f\n",diff / 1000.f);
	//Debugging::Log("ps",log);

	//Update(UPDATE_INTERVAL_S);
	CalcVelocityVector(timeMs);
	Update(float(timeMs / 1000.f));
	//Update(float(1 / CommonDeclarations::GetFPS()));
	
}

void Physical::SaveTo(TiXmlElement * root) const
{
	root->SetAttribute("x", Position.x);
	root->SetAttribute("y", Position.y);
	root->SetAttribute("z", Position.z);
    
	//root->SetAttribute("maxspeed", MaxVelocity);
	//root->SetAttribute("mass", Mass);

    {
        TiXmlElement *elem = new TiXmlElement( "physics" );  
        root->LinkEndChild( elem );

        elem->SetAttribute("maxspeed_x", MaxVelocities.x);
        elem->SetAttribute("maxspeed_y", MaxVelocities.y);
        elem->SetAttribute("maxspeed_z", MaxVelocities.z);

        elem->SetAttribute("stepspeed_x", VelocitiesStep.x);
        elem->SetAttribute("stepspeed_y", VelocitiesStep.y);
        elem->SetAttribute("stepspeed_z", VelocitiesStep.z);
              

        elem->SetAttribute("mass", Mass);        
    }

	if (CollisionResponseModule.get())
		CollisionResponseModule->SaveTo(root);
}

PhysObjDescriptor Physical::GetDescriptor() const 
{
	return Descriptor;
}

void Physical::SetDescriptor(PhysObjDescriptor descr) 
{
	Descriptor = descr;
}

Ogre::Vector3 Physical::GetLinAccelearation() const
{
	return Descriptor.LinAcceleration;
}

Ogre::Vector3 Physical::GetLastVelocity() const
{
	return Descriptor.LastVelocity;
}

Ogre::Matrix3 Physical::GetBoxInertia() const
{
	return BoxInertia;
}

Ogre::Vector3 Physical::GetVelocityVector() const
{
	return Descriptor.VelocityVector;
}

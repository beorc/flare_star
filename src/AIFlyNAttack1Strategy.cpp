#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AIFlyNAttack1Strategy.h"
#include "AIModule.h"
#include "CommonDeclarations.h"
#include "IScenable.h"
#include "Player.h"
#include "Utilities.h"
#include "AIPathFinderStrategy.h"
#include "Debugging.h"
#include "IRoomable.h"

//#define FNAS1_DEBUG

AIFlyNAttack1Strategy::AIFlyNAttack1Strategy(void) : 
AIStrategy(),
prevtime(0),
RotationSpeed(100),
FlyNAttack1State(FA1_NONE),
PrevState(FA1_NONE),
MinDistance(5000),
MaxDistance(30000),
BufferZone(10000),
PrevOrientDirection(Ogre::Vector3::ZERO),
MaxFleeingTime(9999999),
FleeingTime(0)
{	
}

AIFlyNAttack1Strategy::~AIFlyNAttack1Strategy(void)
{
}

void AIFlyNAttack1Strategy::Step(unsigned timeMs)
{		
    /*
     смотрим существует ли цель
     если режим исходный:
     устанавливаем режим подхода
     если режим подхода:
     если цель критически близко - строим вектор отхода за цель (по собств. скорости), не стреляем, смотрим по ходу движения, у подчиненных включаем стратегию смотреть по ходу движения и включаем режим отхода
     если цель на подходящем расстоянии - подходим прямо на нее и атакуем
     если режим отхода:
     если цель на подходящем расстоянии включаем режим подхода
     если цель близко - корректируем вектор скорости
     */
    #ifdef FNAS1_DEBUG
    char log[100];
    #endif
    
    IPhysical *phys = Parent->GetPhysical();
    IScenable *scen = Parent->GetScenable();
	
	if (TargetID<0)
	{		
		return;
	}
	if (TargetID==0)
	{
		if (Owner)
		{
			TargetID = Owner->SelectTargetID();			
		}
		if (TargetID<=0)
		{
			TargetID=-1;
			IEquipped *eq = Parent->GetEquipped();
            if (eq)
            {
                eq->SetTargetPosition(Ogre::Vector3::ZERO);
                eq->SetShooting(false);
            }			
			return;
		}
	}

    IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);
    if (NULL==obj)
    {		
        TargetID=0;
        return;
    }
    IScenable* Target;	
    Target = obj->GetScenable();
    if (NULL==Target)
        return;

	#ifdef FNAS1_DEBUG
    sprintf(log,"start: %d\n", (int)FlyNAttack1State);
    Debugging::Log("fnas",log);
	#endif

    if (FA1_NONE == FlyNAttack1State)
        FlyNAttack1State = FA1_SEEKING;

	Ogre::Vector3 tpos = Target->GetPosition();
	int tradius=300;
    Ogre::Vector3 target_pos = tpos, own_pos = Parent->GetPosition();
    Ogre::Vector3 direction = target_pos - own_pos;
    Ogre::Vector3 orient_direction=Ogre::Vector3::ZERO;
    int actual_rotation_fps = RotationSpeed;
    AIPathFinderStrategy *pf = Owner->GetPathFinder();

	bool is_out_of_rooms = false, is_target_out_of_rooms = false;

	IRoomable *rmbl = obj->GetRoomable();
	if (rmbl)
	{
		IRoomable::RoomsPool *pool = rmbl->GetRooms();
		if (pool->IsEmpty())
			is_target_out_of_rooms = true;
	}

	rmbl = Parent->GetRoomable();
	if (rmbl)
	{
		IRoomable::RoomsPool *pool = rmbl->GetRooms();
		if (pool->IsEmpty())
			is_out_of_rooms = true;
	}

	if (is_out_of_rooms || is_target_out_of_rooms)
	{	
		SwarmStrategy.Step(timeMs);
		return;
	}

	
	{
		PrevState = FlyNAttack1State;

		switch(FlyNAttack1State) {
		case FA1_SEEKING:
			//if (FA1_SEEKING == FlyNAttack1State)
			{
				FleeingTime=0;
				//Ogre::Vector3 target_pos = Target->GetPosition(), own_pos = Parent->GetPosition();
				//Ogre::Vector3 direction = target_pos - own_pos;
				if (direction.squaredLength()<MinDistance*MinDistance)
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"seek close\n");
					Debugging::Log("fnas",log);
					#endif
					FlyNAttack1State = FA1_SEEKING2FLEEING;

					Ogre::Vector3 BoundingPoints[8];
					Target->GetBoundingPoints(Target->GetBoundingBox(false).getSize().z+phys->GetRadius()*2, BoundingPoints);

					IEquipped *eq = Parent->GetEquipped();
					if (eq)
					{
						eq->SetTargetPosition(Ogre::Vector3::ZERO);
						eq->SetShooting(false);
					}

					if (pf)
					{
						Ogre::Vector3 target_point = BoundingPoints[int(7*AAUtilities::Rand())];
						pf->SetTargetPoint(target_point);
						FleeingDirection = target_point - own_pos;
						//orient_direction = target_point - own_pos;
					}
				} else
				{            
					if (direction.squaredLength()<MaxDistance*MaxDistance)
					{
						#ifdef FNAS1_DEBUG
						sprintf(log,"seek go on\n");
						Debugging::Log("fnas",log);
						#endif
						//std::pair<bool, Ogre::Real> intersection_res;
	                    
						IPhysical *phys = obj->GetPhysical();
						IEquipped *eq = Parent->GetEquipped();
						if (phys)
						{
							tpos = eq->CalculateTargetPosition(obj->GetPosition(), phys->GetLastVelocity()/PHYSICS_UPDATE_INTERVAL_MS);
							tradius = phys->GetRadius();
						}                    
						eq->TryToShoot(tpos, tradius);
						Ogre::Vector3 sight_origin = eq->GetSightOrigin();
						sight_origin.z=0;
						tpos -= scen->GetOrientation()*sight_origin;
						direction = tpos - own_pos;
					}      
					if (pf)
					{					
						pf->SetTargetPoint(tpos);
					}
					orient_direction = direction;
					actual_rotation_fps = actual_rotation_fps/4;
				}        
			}
    		break;
		case FA1_SEEKING2FLEEING:
			//if (FA1_SEEKING2FLEEING == FlyNAttack1State)
			{
				FleeingTime+=timeMs;
				if (phys)
					orient_direction = FleeingDirection; //-phys->GetForwardDirection().normalisedCopy();
				/*if (FleeingTime>MaxFleeingTime)
				{
	#ifdef FNAS1_DEBUG
					sprintf(log,"seek2flee to breakaway\n");
					Debugging::Log("fnas",log);
	#endif
					FlyNAttack1State = FA1_BREAKAWAY;
					FleeingTime=0;
				} else*/
				if (direction.squaredLength()>(MinDistance+BufferZone)*(MinDistance+BufferZone))
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"seek2flee switch off\n");
					Debugging::Log("fnas",log);
					#endif
					FlyNAttack1State = FA1_FLEEING;
				} else
				{
					//AIPathFinderStrategy *pf = Owner->GetPathFinder();
					#ifdef FNAS1_DEBUG
					sprintf(log,"seek2flee go on\n");
					Debugging::Log("fnas",log);
					#endif
					//actual_rotation_fps = actual_rotation_fps*4;
					if (pf)
					{
						Ogre::Vector3 target_point = own_pos+orient_direction*100000;
						pf->SetTargetPoint(target_point);                
					}
				}                
			}
    		break;
		case FA1_FLEEING:
			//if (FA1_FLEEING == FlyNAttack1State)
			{
				FleeingTime+=timeMs;
				if (phys)
					orient_direction = -phys->GetForwardDirection();

				//AIPathFinderStrategy *pf = Owner->GetPathFinder();

				if (direction.squaredLength()>(MaxDistance+BufferZone)*(MaxDistance+BufferZone))
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"flee far\n");
					Debugging::Log("fnas",log);
					#endif
					FlyNAttack1State = FA1_FLEEING2SEEKING;                            
				} else
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"flee go on\n");
					Debugging::Log("fnas",log);
					#endif
					if (pf)
					{
						Ogre::Vector3 target_point = own_pos+orient_direction*10000;
						pf->SetTargetPoint(target_point);                
					}
				}
			}
			break;
		case FA1_FLEEING2SEEKING:
			//if (FA1_FLEEING2SEEKING == FlyNAttack1State)
			{
				FleeingTime = 0;
				if (direction.squaredLength()<MaxDistance*MaxDistance)
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"flee2seek switch off\n");
					Debugging::Log("fnas",log);
					#endif
					FlyNAttack1State = FA1_SEEKING;

					/*if (pf)
					{
					Ogre::Vector3 target_point = target_pos;
					pf->SetTargetPoint(target_point);                
					orient_direction = target_point - own_pos;
					}*/
				} else
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"flee2seek go on\n");
					Debugging::Log("fnas",log);
					#endif
					actual_rotation_fps = actual_rotation_fps*5;
					
					if (pf)
					{                
						orient_direction = direction;					
						Ogre::Vector3 target_point = Ogre::Vector3::NEGATIVE_UNIT_Z*10000;
						Ogre::Quaternion orientation = scen->GetOrientation();
						target_point = orientation * target_point;
						target_point = own_pos + target_point;
						pf->SetTargetPoint(target_point);                
					}
					
				}
				//if (phys)
				//    orient_direction = -phys->GetForwardDirection();
			}
			break;    
		case FA1_BREAKAWAY:
			//if (FA1_FLEEING2SEEKING == FlyNAttack1State)
			{
				FleeingTime = 0;
				if (direction.squaredLength()>(MinDistance+BufferZone)*(MinDistance+BufferZone))
				{
					#ifdef FNAS1_DEBUG
					sprintf(log,"BREAKAWAY switch off\n");
					Debugging::Log("fnas",log);
					#endif
					FlyNAttack1State = FA1_FLEEING;
				} else
				{
	#ifdef FNAS1_DEBUG
					sprintf(log,"BREAKAWAY go on\n");
					Debugging::Log("fnas",log);
	#endif
					actual_rotation_fps = actual_rotation_fps*5;

					if (pf)
					{                
						orient_direction = direction;					
						Ogre::Vector3 target_point = Ogre::Vector3::NEGATIVE_UNIT_Z*10000;
						Ogre::Quaternion orientation = scen->GetOrientation();
						target_point = orientation * target_point;
						target_point = own_pos + target_point;
						pf->SetTargetPoint(target_point);                
					}

				}
				//if (phys)
				//    orient_direction = -phys->GetForwardDirection();
			}
			break;
		};

		#ifdef FNAS1_DEBUG
		sprintf(log,"end: %d\n", (int)FlyNAttack1State);
		Debugging::Log("fnas",log);		
		#endif
	}/* else
		{
			actual_rotation_fps = actual_rotation_fps*5;

			if (pf)
			{                
				orient_direction = direction;					
				Ogre::Vector3 target_point = Ogre::Vector3::NEGATIVE_UNIT_Z*10000;
				Ogre::Quaternion orientation = scen->GetOrientation();
				target_point = orientation * target_point;
				target_point = own_pos + target_point;
				pf->SetTargetPoint(target_point);                
			}
		}*/
	
    if (!orient_direction.isZeroLength() && PrevOrientDirection != orient_direction)
    {
        Ogre::Quaternion OurOrientation = scen->GetOrientation();

        //Ogre::Vector3 direction = Target->GetPosition()-scen->GetPosition();

        orient_direction.normalise();

        Ogre::Vector3 up =CommonDeclarations::GetUpVector();
        
        Vector3 xVec = up.crossProduct(orient_direction);
        xVec.normalise();
        Vector3 yVec = orient_direction.crossProduct(xVec);
        yVec.normalise();
        Quaternion unitZToTarget = Quaternion(xVec, yVec, orient_direction);

        Quaternion targetOrientation = Quaternion(-unitZToTarget.y, -unitZToTarget.z, unitZToTarget.w, unitZToTarget.x);

        PrevOrientDirection = orient_direction;        
        RotationUnit.StartRotation(OurOrientation, targetOrientation, actual_rotation_fps);
    }
	
	if(RotationUnit.mRotating)
	{
		RotationUnit.Step();
		if(RotationUnit.mRotating)                                // Process timed rotation
		{				
			Ogre::Quaternion delta = RotationUnit.Slerp();	
			scen->SetOrientation(delta);
		}
	}   
}

//void AIFlyNAttack1Strategy::SetTargetObject(IScenable* obj)
//{
//	Target=obj;
//}

bool AIFlyNAttack1Strategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
	
	AIStrategy::Parse(xml);

	//const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int res;
	
	res = xml->QueryIntAttribute("rotation_speed", &RotationSpeed);

    res = xml->QueryIntAttribute("min_dist", &MinDistance);
    res = xml->QueryIntAttribute("max_dist", &MaxDistance);
    res = xml->QueryIntAttribute("buffer_dist", &BufferZone);

	return TIXML_SUCCESS==res;
}

void AIFlyNAttack1Strategy::Init()
{
	AIStrategy::Init();
	assert(Parent);
	SwarmStrategy.SetParent(Parent);
	SwarmStrategy.SetOwner(Owner);
	IScenable *scen = Parent->GetScenable();
	if (scen)
	{
		SwarmStrategy.SetTargetPosition(scen->GetInitialPosition());
	}
}


#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\airaypathfinderstrategy.h"
#include "CommonDeclarations.h"
#include "IAAObject.h"
#include "Room.h"
#include "RoomsManager.h"
#include "Debugging.h"
#include "IAAObject.h"
#include "IPhysical.h"

#define CHANGE_DIRECTION_TOUT 1000
#define FREE_FLY_TOUT 5000

//#define PATH_FINDER_DEBUG

AIRayPathFinderStrategy::AIRayPathFinderStrategy(void):
AIPathFinderStrategy(),
HaveCashedTarget(false),
HaveTarget(false),
LastChangingDirectionTime(0),
LastTargetPoint(0,0,0),
ClosestObj(NULL),
LinesNumber(0),
Lines(NULL),
Inited(false)
{	
#ifdef PATH_FINDER_NODES_DEBUG
	//for (int i=0;i<LINES;++i)
	//	DebugLinesNodes[i] = Core::GetInstance()->GetSceneManager()->addCubeSceneNode(10.0f,NULL,-1,Vector3(0,0,0),Vector3(0,0,0),Vector3(1,1,1));
	for (int i=0;i<LinesNumber;++i)
	{
		char *guid = CommonDeclarations::GenGUID();
		DebugLinesNodes[i] = CommonDeclarations::GetSceneManager()->createEntity(guid, Ogre::SceneManager::PT_CUBE );
	}
#endif
}

AIRayPathFinderStrategy::~AIRayPathFinderStrategy(void)
{
	if (Lines)
		delete [] Lines;
}

void AIRayPathFinderStrategy::Step(unsigned timeMs)
{
	//if (!Inited)
	//	Init();	
	//Ogre::SceneNode *Node = Parent->GetNode();
	// определить вектор на цель и двигаться по нему с

	ClosestObj = NULL;
	
    // заданной скоростью.

    IPhysical *phys = Parent->GetPhysical();
    assert(phys);

	//if (phys->IsCollided())
	//{		
 //       //int diff=timeMs-StartFreeFlyingTime;
 //       StartFreeFlyingTime+=timeMs;
 //       if (StartFreeFlyingTime>FREE_FLY_TOUT || phys->GetRotVelocity().isZeroLength()) 
 //       {
 //           StartFreeFlyingTime=0;
 //           phys->SetCollided(false);                
 //       }
	//}

	/* 
	если преграда - статический объект, то облетаем и движемся по ВП
	если преграда - целевой объект, то его не облетаем
	если преграда - динамический объект, то облетаем, но не дожидаемся достижения ВП
	*/
	Vector3 point;
	static Vector3 prev_forward_direction(0,0,0);

    AIPathFinderStrategy::Step(timeMs);

	if (TargetType != TT_NONE) 
	{	
		if (HaveCashedTarget)
		{
			point = CashedTargetPoint;
			Vector3 dist = point-Parent->GetScenable()->GetPosition();
			//if (dist.squaredLength()>100.f*100.f)
            if (!IsTargetReached(CashedTargetPoint))
			{                
                LastChangingDirectionTime+=timeMs;
				if (ClosestObj && ClosestObj->GetScenable()->GetType()==PT_DYNAMIC && LastChangingDirectionTime>CHANGE_DIRECTION_TOUT) 
				{   
                    LastChangingDirectionTime = 0;
					HaveCashedTarget = false;
					point = RouteToTarget();					
				} else
					Route(point);
			}
			else
			{
				HaveCashedTarget = false;
				point = RouteToTarget();
			}
		} else
		{
			point = RouteToTarget();
		}
		if (LastTargetPoint!=point)
		{
			LastChangingDirectionTime = 0;
			LastTargetPoint = point;
		}
	}	
}

Vector3 AIRayPathFinderStrategy::RouteToTarget()
{
    Ogre::Vector3 point;
	//if (TargetType == TT_OBJECT) {
	//	//assert(TargetID);
	//	Ogre::Vector3 parent_pos = Parent->GetPosition();

	//	if (TargetID<0)
	//	{
	//		assert(false);
	//		TargetPoint = parent_pos;
	//	}
	//	
	//	if (TargetID==0)
	//	{
	//		if (Owner)
	//		{
	//			TargetID = Owner->SelectTargetID();			
	//		}
	//		if (TargetID<=0)
	//		{
	//			//assert(false);
	//			TargetPoint = parent_pos;
	//		}
	//	}
 //       
	//	IAAObject *obj = CommonDeclarations::GetIDObject(TargetID);
	//	//assert(obj);
	//	if (obj)
	//	{
	//		Ogre::Vector3 target_pos = obj->GetPosition();
	//		if (DistanceToTarget>0)
	//			TargetPoint = target_pos + (parent_pos-target_pos).normalisedCopy()*DistanceToTarget;
	//		else
	//			TargetPoint=target_pos;
	//	} else
	//	{
	//		//if (--WaitTargetTimeout<0)
	//		//	CommonDeclarations::DeleteObjectRequest(Parent);
	//		TargetPoint = parent_pos;
	//		//CommonDeclarations::DeleteObjectRequest(Parent);
	//	}
	//}/* else
	//if (TargetType == TT_OBJECT)
	//{
	//	point = TargetPoint;
	//}*/
    
	point = TargetPoint;
	Vector3 dist = point-Parent->GetPosition();
	if (dist.squaredLength()>10.f*10.f)
		Route(point);
	else
	{
		if (TargetType==TT_POINT)
		{
			TargetType = TT_NONE;	
			State = FST_POINTREACHED;
		}		
		Parent->GetPhysical()->SetThrottle(Vector3(0,0,0));		
		Parent->GetPhysical()->SetForces(Ogre::Vector3::ZERO);
		//Parent->SetVelocityVector(Vector3(0,0,0));
	}
	return point;
}

void AIRayPathFinderStrategy::CalcLines(Ogre::Ray &line)
{
	Vector3 start;
	Vector3 direction = line.getDirection();
	Ogre::Quaternion quat;
	for (int i=0;i<LinesNumber-1;++i)
	{
		start = LineOrigins[i];			
		quat = start.getRotationTo(direction);

		start = quat*start; //matr.rotateVect(lines[i].start);
		Lines[i].setOrigin(start+line.getOrigin()); //lines[i].start += line.start;
		Lines[i].setDirection(start+direction); //lines[i].end = lines[i].start+v;		
	}

	Lines[LinesNumber-1] = line;
}

bool AIRayPathFinderStrategy::IsAccessible(Ogre::Ray line, List<Room*> &EdgeRooms)
{
	//std::vector<Room*>::iterator iPosRoom=EdgeRooms.begin(), iEndRoom=EdgeRooms.end();
	//Room::ObjectsPool::iterator iPosObj, iEndObj;
	//Room::ObjectsPool::ListNode *pos=NULL;	
	Room::ObjectsPool *pool;	
	Ogre::AxisAlignedBox NodeBox, OurBox;
	Vector3 center;

	CalcLines(line);

#ifdef PATH_FINDER_NODES_DEBUG
	for (int i=0;i<LinesNumber;++i)
		DebugLinesNodes[i]->setPosition(Lines[i].start);
#endif

    Room::ObjectType *obj;
	IScenable *scen=NULL;
	for (List<Room*>::ListNode *pos=EdgeRooms.GetBegin();pos!=NULL;pos=pos->Next)
	{
		pool=pos->Value->GetChildObjects();
		//iPosObj=pool->begin();
		//iEndObj=pool->end();
		//for (;iPosObj!=iEndObj;++iPosObj)
		
		for (Room::ObjectsPool::ListNode *pos = pool->GetBegin();pos!=NULL;pos=pos->Next)
		{
			obj = pos->Value;
			scen = obj->GetScenable();
			if (obj==Parent || NULL==scen) {
				continue;
			}

			NodeBox = scen->GetBoundingBox(true);
			
			for (int i=0;i<LinesNumber;++i)
			{
				std::pair< bool, Real > res = Math::intersects(Lines[i], NodeBox);
				if (res.first)
				{	
					return false;					
				}
			}			
		}
	}
	return true;
}

bool AIRayPathFinderStrategy::GetIntersectionObjects(Ogre::Ray line, List<Room*> &EdgeRooms, List<ObjZBuffer> *ObjBuffer)
{	
	//std::vector<Room*>::iterator iPosRoom=EdgeRooms.begin(), iEndRoom=EdgeRooms.end();
	//Room::ObjectsPool::iterator iPosObj, iEndObj;
	Room::ObjectsPool *pool;	
	Ogre::AxisAlignedBox NodeBox, OurBox;
	ObjZBuffer tmp;

	CalcLines(line);

#ifdef PATH_FINDER_NODES_DEBUG
	for (int i=0;i<LinesNumber;++i)
		DebugLinesNodes[i]->setPosition(Lines[i].start);
#endif


	//std::vector<ObjZBuffer>::iterator iPosBuf, iEndBuf;
	bool AlreadyInBuf;
    Room::ObjectType *obj;
	IScenable *scen=NULL;

	for (List<Room*>::ListNode *room_pos = EdgeRooms.GetBegin();room_pos!=NULL;room_pos=room_pos->Next)
	{
		pool=room_pos->Value->GetChildObjects();
		//iPosObj=pool->begin();
		//iEndObj=pool->end();
		//for (;iPosObj!=iEndObj;++iPosObj)
		for (Room::ObjectsPool::ListNode *obj_pos = pool->GetBegin();obj_pos!=NULL;obj_pos=obj_pos->Next)
		{
			obj = obj_pos->Value;
			scen = obj->GetScenable();
			if (obj==Parent || NULL==scen) {
				continue;
			}
			

			// проверить, что уже нет в списке такого объекта
			
			AlreadyInBuf=false;
			for (List<ObjZBuffer>::ListNode *pos=ObjBuffer->GetBegin();pos!=NULL;pos=pos->Next)
			{
				if (pos->Value.object==obj) {
					AlreadyInBuf=true;
					break;
				}
			}
			if (AlreadyInBuf) {
				continue;
			}

			
			NodeBox = scen->GetBoundingBox(true);			
			std::pair< bool, Real > res;


			for (int i=0;i<LinesNumber;++i)
			{
				res = Math::intersects(Lines[i], NodeBox);
				if (res.first)
				{						
					tmp.distance = res.second;
					tmp.object=obj;							
					ObjBuffer->PushBack(tmp);
					break;					
				}
			}			
		}
	}
	return true;
}

void AIRayPathFinderStrategy::GoTo(Vector3 point)
{
	Ogre::Vector3 Position = Parent->GetPosition();
	Ogre::Vector3 vr;
	//vr=Position-point;

	Parent->GetScenable()->GetOrientation();

	vr=Parent->GetScenable()->GetOrientation()*Ogre::Vector3::UNIT_Z;


	Vector3 AccelerationOn(Vector3::ZERO); // = Parent->GetAccelerationOn();
    	IPhysical *phys = Parent->GetPhysical();
    	assert(phys);

	
	vr.normalise();
	phys->SetForwardDirection(vr);		
	AccelerationOn.z=-1;
	//phys->SetVelocityVector(vr);
	
	phys->SetAcceleration(AccelerationOn);
}

void AIRayPathFinderStrategy::Route(Vector3 point)
{
	/*
	среди всех окружающих комнат получить те, которые пересекает
	направ. луч. перебрать объеты нашей комнаты и отобранных выше
	комнат на предмет пересечения их ббокса лучем. Среди всех
	этих объектов выбираем ближайший к нам: 
	а) постепенно наращивая луч, ждем первого пересечения с одним из выбранных выше объектов.
	б) находим точку пересечения с описанной вокруг ббокса сферой и определяем объект с ближайшей
	точкой

	для объекта выбираем вэйпойнт, ближайший к цели и видимый нам
	когда достигаем вэйпойнта, повторяем сначала	 
	*/

	//выбираем комнату, в которой находится наш центр (nose)


    IScenable *scen = Parent->GetScenable();
    assert(scen);

	Vector3 Position = scen->GetPosition();	
	Room *room=NULL;
	List<Room*> EdgeRooms;	

	Vector3 v, PreTargetPoint;	
	float HalfSide;
	IScenable *ClosestObj1=NULL;
	
	HalfSide = scen->GetBoundingBox().getSize().z/2; //test
	v=TargetPoint;
	v.normalise();
	
	Ogre::Ray line(Position,point);
	bool HaveIntersection=false;

    IRoomable *roomable = Parent->GetRoomable();
    assert(roomable);

	room = roomable->GetRoom(scen->GetPosition());

	if (NULL==room) {
		GoTo(point); // если мы за комнатами, препятствия не облетаем, нужно быстрее вернуться в комнаты.
        	//CommonDeclarations::GetRoomManager()->UpdateObject(Parent);
		//room = roomable->GetRoom(scen->GetPosition());

        //return;
		//assert(NULL!=room);
		return;
		/*
		char log[100];
		sprintf(log,"AI Path finder: A room not found\n");
		Debugging::Log("Warnings.log",log);
		descriptor->Throttle= Vector3(0,0,0);
		return;
		*/
	}

	// выбираем из окружающих комнат те, что пересекаются с нашим лучем (дорожкой)
	//if (!EdgeRooms.IsEmpty()) {
		EdgeRooms.Clear();
	//}
	Room *ActiveRoom = room->GetParentRoom();
	assert(ActiveRoom);
	if (NULL==ActiveRoom)
	{
		return;
	}
	ActiveRoom->GetEdgeRooms(room->GetRoomID(),EdgeRooms,line);

	EdgeRooms.PushBack(room);
	// перебираем объекты вблизи нашего пути, с которыми луч пересекается и ищем ближайший

	List<ObjZBuffer> ObjBuffer;
	ObjBuffer.Clear();

	GetIntersectionObjects(line, EdgeRooms, &ObjBuffer);

	// если путь свободен, движемся к ВП или к цели
	if (ObjBuffer.IsEmpty()) {		
#ifdef PATH_FINDER_DEBUG
		Debugging::Log("lines","intersection not found\n");
#endif
		//descriptor->Velocity= Vector3(0,0,0);
		//return;
		//break;
	} else
	{
		//std::vector<ObjZBuffer>::iterator iPosZ=ObjBuffer.begin(), iEndZ=ObjBuffer.end();
		long long SmallestDist=10000000;			

		for (List<ObjZBuffer>::ListNode *pos=ObjBuffer.GetBegin();pos!=NULL;pos=pos->Next)
		{
			if (pos->Value.distance<SmallestDist)
			{
				SmallestDist=pos->Value.distance;
				ClosestObj=pos->Value.object;
			}
		}

		// если какой-то объект преграждает путь, идем по ВП, предварительно проверив
		// путь для нового направления (чтобы он был свободным).
		// если путь свободен, идем к точке с нашей скоростью

		
		if (NULL!=ClosestObj) 
		{
			IScriptable *scr = ClosestObj->GetScriptable();
			IScenable *closest_scen = ClosestObj->GetScenable();
			if (scr==NULL || scr->GetID()!=TargetID)
			{
				// выбираем для объекта все видимые нам ВП				

				SmallestDist=10000000;
				long long dist;

				ClosestObj1 = closest_scen;

				{
					Vector3 BoundingPoints[8];
					closest_scen->GetBoundingPoints(2*HalfSide, BoundingPoints);					

					for (int i=0;i<8;++i)
					{
						line.setOrigin(Position);
						line.setDirection(BoundingPoints[i]);

						if (IsAccessible(line, EdgeRooms))
						{
							dist=(point-line.getDirection()).squaredLength();
							if (dist<SmallestDist)
							{	
								HaveIntersection=true;
								SmallestDist=dist;								
								PreTargetPoint=BoundingPoints[i];								
#ifdef PATH_FINDER_DEBUG
								Debugging::Log("lines","dist looking...\n");
#endif
							}
						}
					}	
				}
			} else
			{
				
#ifdef PATH_FINDER_DEBUG
					Debugging::Log("Warnings.log","closest object not found\n");
#endif
					//Parent->GetPhysical()->SetForwardDirection(Vector3(0,0,0));
					//return;				
			}
			
		} else
		{			
			if (NULL==ClosestObj)
			{
				#ifdef PATH_FINDER_DEBUG
				Debugging::Log("Warnings.log","closest object not found\n");
				#endif
				Parent->GetPhysical()->SetForwardDirection(Vector3(0,0,0));
				return;
			}								
		}
	}

	if (!HaveIntersection && ClosestObj1)
	{
		#ifdef PATH_FINDER_DEBUG
		Debugging::Log("Warnings.log","access to closest object not found\n");
		#endif
		Parent->GetPhysical()->SetForwardDirection(Vector3(0,0,0));
		return;
	}

	Vector3 vr;
	if (HaveIntersection) 
	{			
		HaveCashedTarget = true;
		CashedTargetPoint = PreTargetPoint;


		//vr=Position-CashedTargetPoint;	
		GoTo(CashedTargetPoint);
	} else	
	{		
		//vr=Position-point;
		GoTo(point);
	}

/*	Vector3 AccelerationOn(Vector3::ZERO); // = Parent->GetAccelerationOn();
    IPhysical *phys = Parent->GetPhysical();
    assert(phys);

	{
		vr.normalise();
		phys->SetForwardDirection(vr);		
		AccelerationOn.z=-1;
		//phys->SetVelocityVector(vr);
	}
	phys->SetAcceleration(AccelerationOn);
*/

#ifdef PATH_FINDER_DEBUG
	if (HaveCashedTarget) 
	{
		char log[200];
		sprintf(log, "%d going to WP (%f,%f,%f)\n",Parent->GetScriptable()->GetID(),CashedTargetPoint.x, CashedTargetPoint.y, CashedTargetPoint.z);
		Debugging::Log("lines",log);	
	} else
	{
		char log[200];
		sprintf(log, "%d going to TARGET (%f,%f,%f)\n",Parent->GetScriptable()->GetID(),point.x, point.y, point.z);
		Debugging::Log("lines",log);	
		//Debugging::Log("lines","going to TARGET\n");		
	}
#endif

}

bool AIRayPathFinderStrategy::Parse(TiXmlElement* xml)
{	
	if (xml == 0)
		return false; // file could not be opened

	AIPathFinderStrategy::Parse(xml);

	int res;
	res = xml->QueryIntAttribute("raysnum", &LinesNumber);	
	assert(TIXML_SUCCESS==res);
	
	return true;
}

void AIRayPathFinderStrategy::Init()
{
	AIPathFinderStrategy::Init();
	//if (Inited)
	//	return;
	Inited=true;
	if (Lines)
		delete [] Lines;
	
	Lines = new Ogre::Ray[LinesNumber];

    IScenable *scen = Parent->GetScenable();
    assert(scen);

	Ogre::AxisAlignedBox OurBox = scen->GetBoundingBox(false);
	Vector3 extent = OurBox.getSize();
	float HalfSide = extent.z/2, Side=HalfSide*2;

	LineOrigins[0]=Vector3(-HalfSide,-HalfSide,0);
	LineOrigins[1]=Vector3(-HalfSide,HalfSide,0);
	LineOrigins[2]=Vector3(HalfSide,HalfSide,0);
	LineOrigins[3]=Vector3(HalfSide,-HalfSide,0);

	LineOrigins[4]=Vector3(0,-HalfSide,0);
	LineOrigins[5]=Vector3(-HalfSide,0,0);
	LineOrigins[6]=Vector3(0,HalfSide,0);
	LineOrigins[7]=Vector3(HalfSide,0,0);
}




#include "StdAfx.h"
#include ".\room.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Debugging.h"
#include "SceneObject.h"
#include "IRoomable.h"

#include "CommonDeclarations.h"

Room::Room(void):
Box(0,0,0,0,0,0),
ParentRoom(NULL),
RoomID(0),
Objects(100),
ChildSquaredSize(0),
ChildSideLength(0)
{
	#ifdef ROOMS_SHOW_DEBUG_DATA
    memset(DebugNodes,0,sizeof(DebugNodes));
    memset(DebugEntities,0,sizeof(DebugEntities));
	#endif
}

Room::Room(RoomBoxType box) : 
ParentRoom(NULL),
Objects(100),
ChildSquaredSize(0),
ChildSideLength(0)
{
	#ifdef ROOMS_SHOW_DEBUG_DATA
    memset(DebugNodes,0,sizeof(DebugNodes));
    memset(DebugEntities,0,sizeof(DebugEntities));
	#endif
	Init(box);
}

Room::~Room(void)
{
	//for (size_t i=0;i<Childs.Size;++i)
	//	Childs.Data[i]->~Room();
#ifdef ROOMS_SHOW_DEBUG_DATA
    Ogre::SceneManager *smgr = CommonDeclarations::GetSceneManager();
    for (int i=0;i<8;++i)
    {
        if (DebugNodes[i])
        {
            smgr->destroySceneNode(DebugNodes[i]->getName());		
        }	
        if (DebugEntities[i])
        {
            smgr->destroyMovableObject(DebugEntities[i]);		
        }
    }  
#endif
}

void Room::Init(RoomBoxType box, unsigned id)
{
	Box=box;
	RoomID=id;

	#ifdef ROOMS_SHOW_DEBUG_DATA

    const Ogre::Vector3 *edges = box.getAllCorners();
    Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();
    
    for (int i=0;i<8;++i)
    {        
        char *guid = CommonDeclarations::GenGUID();
        //sprintf(name,"RDO%s",guid);
        DebugEntities[i] = SceneMgr->createEntity(guid, Ogre::SceneManager::PT_CUBE);

        if (DebugEntities[i])
        {		
            //if (NULL==DebugNodes[i])
                DebugNodes[i] = SceneMgr->getRootSceneNode()->createChildSceneNode();
///            DebugNodes[i]->attachObject(DebugEntities[i]);
            DebugNodes[i]->setPosition(edges[i]);
        }
    }

	#endif

	Ogre::Vector3 v = Box.getSize();	
	SideLength = (unsigned)v.z;	
}

void Room::Divide(unsigned div)
{
	/*   

	| Z  
	|   / Y
	|  /
	| /
	|/________X

	*/
	Divider=div;
	
	ChildSideLength = SideLength/div;    	
	if (ChildSideLength<10)
		return;

	unsigned ChildsCount=div*div*div, x=0,y=0,z=0, i=0;
	RoomBoxType box;
	Ogre::Vector3 MinEdge;

	ChildSquaredSize = 3*(ChildSideLength*ChildSideLength);

	Childs.Resize(ChildsCount);
	for (z=0;z<div;++z)	
		for (y=0;y<div;++y)
			for (x=0;x<div;++x)
			{	
				MinEdge = Box.getMinimum(); //test
				box.setMinimum(MinEdge.x + x*ChildSideLength,
							   MinEdge.y + y*ChildSideLength,
							   MinEdge.z + z*ChildSideLength);

				MinEdge = box.getMinimum();
				box.setMaximum(MinEdge.x + ChildSideLength,
							   MinEdge.y + ChildSideLength,
							   MinEdge.z + ChildSideLength);
								
				Childs.Data[i].Init(box,i);
				Childs.Data[i].SetParentRoom(this);
				++i;
			}	
}

void Room::AddObject(ObjectType* object)
{		
    if (object->GetRoomable()->GetStoreInRoom())
		Objects.PushBack(object);
		
	object->GetRoomable()->AddRoom(this);


	#ifdef ROOMS_DEBUG
	char log[100];
	sprintf(log,"%d added to %d\n",object->GetScriptable()->GetID(),RoomID);
	Debugging::Log("rooms",log);
	#endif

	//return nMaxHandle;	
}

void Room::RemoveObject(ObjectType* object)
{
#ifdef ROOMS_DEBUG
			char log[100];
			sprintf(log,"%d removed from %d (%d)\n",object->GetScriptable()->GetID(),RoomID,object->GetRoomable()->GetRooms()->GetSize());
			Debugging::Log("rooms",log);
#endif			
            if (object->GetRoomable()->GetStoreInRoom())
				Objects.Remove(object);	
}

RoomBoxType *Room::GetBox()
{
	return &Box;
}

RoomBoxType *Room::GetBox(unsigned i)
{
	return Childs.Data[i].GetBox();
}

bool Room::PutObject(ObjectType* object)
{
	bool found=false;

	Ogre::AxisAlignedBox box = object->GetRoomable()->GetBoundingBox();

	//object->RemoveFromRooms();
	//object->GetRooms()->clear();

	if (Childs.IsEmpty() && Box.intersects(box))
	{		
		AddObject(object);
		return true;
	}

	//ChildRoomsPool::iterator iPos = Childs.begin(), iEnd = Childs.end();
	Room *room=NULL;

	//for (;iPos!=iEnd;++iPos)
	for (unsigned i=0;i<Childs.Size;++i)
	{
		room = &Childs.Data[i];
		if (room->GetBox()->intersects(box)) 
		{
            room->AddObject(object);
			found=true;
		}
	}	

	return found;
}

bool Room::UpdateObject(ObjectType* object)
{
	/*   
	
	| Z  
	|   / Y
	|  /
	| /
	|/________X
	
	*/
	bool found=false;
	
	IRoomable::RoomsPool *pool = object->GetRoomable()->GetRooms();

	if (pool->IsEmpty())
	{
		PutObject(object); 
		pool = object->GetRoomable()->GetRooms();

		if (pool->IsEmpty()) 
		{
            return false;
            /*
			char log[100];
			sprintf(log,"Rooms: empty room set for object %d\n",object->GetID());
			Debugging::Log("Warnings.log",log);
            */
		}		

	}

	Ogre::AxisAlignedBox &box = object->GetRoomable()->GetBoundingBox(), *room_box;	
	const Ogre::Vector3 *edges = box.getAllCorners();

	//IRoomable::RoomsPool::iterator iPos = pool->begin(), iEnd = pool->end();
	IRoomable::RoomsPool::ListNode *temp_pos;
	
	bool alreadyin, huge_object=false;
	int outroom;
	Room *room;
	Ogre::AxisAlignedBox *RoomBox;

	List<Room*> RoomsToAdd;

	Ogre::Real sz = box.getSize().squaredLength();
	if (sz>ChildSquaredSize)
		huge_object=true;
		
	for (IRoomable::RoomsPool::ListNode *pos = pool->GetBegin();pos!=NULL;)
	{
		room_box = pos->Value->GetBox();
		outroom=0;
		found = true;
		if (!huge_object)
		{
			for (int i=0;i<8;++i)
			{
				if (!room_box->contains(edges[i])) {
					++outroom;				
				}
			}
		} else
		{
			if (room_box->intersects(box)) 
			{
				if (!box.contains(*room_box)) 
					outroom = 4;
			} else
			{
				outroom = 8;
			}
		}
		
		//if (!room_box.intersectsWithBox(box)) // если вышли из комнаты
		if (outroom)
		{
			found = false;
			// проверяем соседние комнаты
			unsigned id = pos->Value->GetRoomID(), num, x, y, z, square, i, j, k;
			unsigned start_k, start_j, start_i;
			//Room *edges[26];
			square = Divider*Divider;
			z = id/square;
			y = (id - z*square) / Divider;			
			x = id - z*square - y*Divider;
			
			start_k = z==0 ? 1 : z-1;
			start_j = y==0 ? 1 : y-1;
			start_i = x==0 ? 1 : x-1;
						
			for (k=start_k;k<z+2 && k<Divider;++k)	
				for (j=start_j;j<y+2 && j<Divider;++j)
					for (i=start_i;i<x+2 && i<Divider;++i)
					{
						if (i==x && j==y && k==z)
							continue;						
							
						num = k*square + j*Divider + i;
						room = &Childs.Data[num];
						
						RoomBox = room->GetBox();
						if (RoomBox->intersects(box))
						{
                            found=true;							
							alreadyin=false;

							for (List<Room*>::ListNode *pos = RoomsToAdd.GetBegin(); pos!=NULL; pos=pos->Next)
							{
								if (pos->Value==room)
								{                                    
									alreadyin=true;
									break;
								}								
							}
							if (!alreadyin && !object->GetRoomable()->IsRoomParent(room))
                                RoomsToAdd.PushBack(room); //room->AddObject(object);                                					
						}
					}			

			if (8==outroom) 
			{				
				pos->Value->RemoveObject(object);
				temp_pos = pos;
				pos = pos->Next;
				pool->Remove(temp_pos);
			}			
		}
/*
		if (!found)
		{
			char log[100];	
			sprintf(log,"side room nf room: %d, obj: %d \n",RoomID,object->GetID());
			Debugging::Log("Warnings.log",log);
		}
*/
		if (outroom<8)
		{
			pos=pos->Next;
		}
	}	
	
	for (List<Room*>::ListNode *pos = RoomsToAdd.GetBegin(); pos!=NULL; pos=pos->Next)	
	{		
		pos->Value->AddObject(object);
	}

	return true;
}

void Room::GetEdgeRooms(unsigned id, List<Room*> &OutRooms, Ogre::Ray line)
{
	/*   

	| Z  
	|   / Y
	|  /
	| /
	|/________X

	*/
		
	Room *room;

	
			// проверяем соседние комнаты
			unsigned num, x, y, z, square, i, j, k;
			//Room *edges[26];
			square = Divider*Divider;
			z = id/square;
			y = (id - z*square) % Divider;
			x = id - z*square - y*Divider;

			unsigned start_k, start_j, start_i;
			
			start_k = z==0 ? 1 : z-1;
			start_j = y==0 ? 1 : y-1;
			start_i = x==0 ? 1 : x-1;

			for (k=start_k;k<z+2 && k<Divider;++k)	
				for (j=start_j;j<y+2 && j<Divider;++j)
					for (i=start_i;i<x+2 && i<Divider;++i)
					{
						if (i==x && j==y && k==z)
							continue;
						num = k*square + j*Divider + i;
						room = &Childs.Data[num];

						if (line.intersects(*room->GetBox()).first)
						{
							OutRooms.PushBack(room);
						}
					}	
}

void Room::UpdateDebugInfo()
{
	/*
	if (DebugNode==NULL && Childs.empty())
	{
		Vector3 center;
				
		center.x=(float)(Box.MinEdge.x + Box.MaxEdge.x) / 2.0f;
		center.y=(float)(Box.MinEdge.y + Box.MaxEdge.y) / 2.0f;
		center.z=(float)(Box.MinEdge.z + Box.MaxEdge.z) / 2.0f;
		
		DebugNode = Core::GetInstance()->GetSceneManager()->addCubeSceneNode((float)SideLength,0,-1,center,Vector3(0,0,0),Vector3(1,1,1));
		video::SMaterial mat;		
		//mat.Shininess = 120.0f; // set size of specular highlights
		mat.Lighting = false;
		mat.Wireframe = true;
		//mat.MaterialType = video::EMT_TRANSPARENT_ADD_COLOR;//EMT_SOLID
		DebugNode->getMaterial(0) = mat;
		DebugNode->setVisible(false);
	}
	
	ChildRoomsPool::iterator iPos = Childs.begin(), iEnd = Childs.end();

	for (;iPos!=iEnd;++iPos)
	{
		iPos->UpdateDebugInfo();
	}
*/
}

unsigned	 Room::GetRoomID()
{
	return RoomID;
}

Room::ChildRoomsPool *Room::GetChildRooms()
{
	return &Childs;
}

Room::ObjectsPool *Room::GetChildObjects()
{
	return &Objects;
}

Room *Room::GetRoom(Ogre::Vector3 point)
{
	if (!Box.contains(point))
		return NULL;
	Room *room;

	//ChildRoomsPool::iterator iPos = Childs.begin(), iEnd = Childs.end();

	//for (;iPos!=iEnd;++iPos)
	for (unsigned i=0;i<Childs.Size;++i)
	{
		room = &Childs.Data[i];
		if (room->GetBox()->contains(point)) 
		{
			return room;
		}
	}	
	return NULL;
}

Room *Room::GetParentRoom()
{
	return ParentRoom;
}

void Room::SetParentRoom(Room *parent)
{
	ParentRoom = parent;
}

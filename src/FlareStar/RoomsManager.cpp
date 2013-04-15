#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\roomsmanager.h"

#include "Room.h"
#include "IRoomable.h"
#include "IAAObject.h"
#include "Debugging.h"
#include "AACamera.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "Player.h"

RoomsManager::RoomsManager(void):
OuterObjectsNumber(0)
{
	OuterObjects.Resize(500);
	ClearOuterObjects();
}

RoomsManager::~RoomsManager(void)
{
   ClearRooms();
}

void RoomsManager::ClearRooms()
{    
    /*for (size_t i=0;i<Rooms.Size;++i)
    {
        delete Rooms.Data[i];
    }*/
	for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
	{
		delete pos->Value;
	}
	Rooms.Clear();
}

bool RoomsManager::Parse(TiXmlElement* xml)
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

    str = xml->Value();
    assert(strcmp(str,"rooms")==0);

    int size, detail, res;    
    Room *temp_room;
    TiXmlElement *xml_element = 0;
    xml_element = xml->FirstChildElement("room");
	int room_count=0;

    //std::vector<Room*> rooms;
    while (xml_element)
    {		        
        res = xml_element->QueryIntAttribute("size", &size);
        assert(TIXML_SUCCESS==res);
        res = xml_element->QueryIntAttribute("detail", &detail);
        assert(TIXML_SUCCESS==res);

		int x, y, z;
		res = xml_element->QueryIntAttribute("x", &x);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryIntAttribute("y", &y);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryIntAttribute("z", &z);
		assert(TIXML_SUCCESS==res);

        RoomBoxType box(-size+x,-size+y,-size+z, size+x,size+y,size+z);
        temp_room = new Room;
        temp_room->Init(box, room_count++);
        temp_room->Divide(detail);		
        Rooms.PushBack(temp_room);
        
        xml_element = xml_element->NextSiblingElement("room");
    }

    /*Rooms.Resize(rooms.size());    
    memset(ActualRooms.Data,0,sizeof(ActualRooms.Data));
    std::vector<Room*>::iterator iPos = rooms.begin(), iEnd = rooms.end();
    for (size_t i=0;iPos!=iEnd;++iPos,++i)
    {
        Rooms.Data[i]=*iPos;
    }
	*/
    
    return true;
}

bool RoomsManager::PutObject(Room::ObjectType* object)
{
    //return false;
    object->GetRoomable()->RemoveFromRooms();	
	object->GetRoomable()->GetRooms()->Clear();

	bool updated=false, inrooms=false;
	Room *room;
	//std::vector<Room*>::iterator iPos=Rooms.begin(), iEnd=Rooms.end();
	/*for (size_t i=0;i<Rooms.Size;++i)
	{
		room = Rooms.Data[i];
		updated = room->PutObject(object);
		if (updated)
		{						
			inrooms = true;			
		}
	}*/

    Ogre::AxisAlignedBox box = object->GetRoomable()->GetBoundingBox(), *RoomBox;
        
	for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
	{	
		room = pos->Value;
        RoomBox = room->GetBox();
        if (RoomBox->intersects(box))
        {
            updated = room->PutObject(object);
            if (updated)
            {						
                inrooms = true;			
            }
        }		
	}

	/*if (!inrooms && object->GetTag()!=T_CAMERA)
	{
		char log[100];
		sprintf(log,"Rooms 0: empty room set for object %d\n",object->GetScriptable()->GetID());
		Debugging::Log("Warnings.log",log);
	}*/

	return inrooms;
}

bool RoomsManager::UpdateObject(Room::ObjectType* object)
{    
    bool updated=false, inrooms=false;
    Room *room;

    //std::vector<Room*>::iterator iPos=Rooms.begin(), iEnd=Rooms.end();
	inrooms=false;
    //for (size_t i=0;i<Rooms.Size;++i)
    Ogre::AxisAlignedBox box = object->GetRoomable()->GetBoundingBox(), *RoomBox;
    
	//assert()

	for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
    {
        room = pos->Value;        
        RoomBox = room->GetBox();
        if (RoomBox->intersects(box))
        {
            updated = room->UpdateObject(object);
            if (updated)
		    {           
			    inrooms=true;			
		    }
        }
    }

	if (!inrooms)
	{
		/*char log[100];
		sprintf(log,"Rooms 1: empty room set for object %d\n",object->GetScriptable()->GetID());
		Debugging::Log("Warnings.log",log);*/		
				
		IRoomable *rmbl = object->GetRoomable();
		IPhysical *phys = object->GetPhysical();
		if (rmbl)
		{
			rmbl->RemoveFromRooms();
			rmbl->GetRooms()->Clear();			
			switch (rmbl->GetRoomOnly())
			{
				case IRoomable::ROM_RESTORE:
					{						
						for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
						{
							Ogre::AxisAlignedBox *box = pos->Value->GetBox();
							Ogre::Vector3 center = box->getCenter();
							Ogre::Vector3 position = object->GetPosition();
							Ogre::Vector3 dist = position - center;

							int sqradius = AAUtilities::f2i(box->getHalfSize().squaredLength())+3*phys->GetRadius();
														
							int sqdist = AAUtilities::f2i(dist.squaredLength());
							if (sqradius>sqdist)
							{
								//GetPhysical()->SetForwardDirection(GetOrientation()*Vector3::UNIT_Z);
								//object->GetPhysical();
								//object->RestoreBackupPosition();								
								//Ogre::Vector3 newdir=phys->GetLastVelocity();
								//phys->Stop();
								phys->SetReplacingDirection(-dist.normalisedCopy()*10);
								break;
							}
						}
						AddOuterObject(object);
						//object->RestoreBackupPosition();
						break;
					}					
				case IRoomable::ROM_DESTROY:
					{
						CommonDeclarations::DeleteObjectRequest(object);
						break;
					}					
				case IRoomable::ROM_SCRIPT:
					{
						IScriptable *scr = object->GetScriptable();
						if (scr && scr->GetID()>0)
							ScriptManager::GetInstance()->Call("OnOutOfRooms", "i", false, object->GetScriptable()->GetID());
						break;
					}
				case IRoomable::ROM_NONE:
					{
						AddOuterObject(object);
						break;
					}

			};
		}
		
	}
    
    return inrooms;
}

size_t RoomsManager::GetOuterObjectsNumber()
{
	return OuterObjectsNumber;
}

inline void RoomsManager::ClearOuterObjects()
{
	OuterObjectsNumber = 0;
	for (size_t i=0;i<OuterObjects.Size;++i)
		OuterObjects.Data[i]=NULL;
}

RoomsManager::OuterObjectsContainer *RoomsManager::GetOuterObjects()
{
	return &OuterObjects;
}

inline void RoomsManager::AddOuterObject(IAAObject *object)
{
	if (OuterObjectsNumber < OuterObjects.Size) 
	{
		OuterObjects.Data[OuterObjectsNumber] = object;
		++OuterObjectsNumber;
	}
}

bool RoomsManager::IsValidPosition(Ogre::Vector3 position)
{
	bool found=false;
	Room *room;
	//std::vector<Room*>::iterator iPos=Rooms.begin(), iEnd=Rooms.end();
	//for (size_t i=0;i<Rooms.Size;++i)
	for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
	{
		room = pos->Value;
		found = room->GetBox()->contains(position);
		if (found)
		{
			return true;
		}
	}

	return false;
}

RoomsManager::RoomsPool *RoomsManager::GetRooms()
{
	return &Rooms;
}

Room *RoomsManager::GetRoom(const Ogre::Vector3 &point)
{
	Room *room=NULL, *res=NULL;
	//RoomsPool::iterator iPos=Rooms.begin(), iEnd=Rooms.end();	
	//for (size_t i=0;i<Rooms.Size;++i)
	for (RoomsPool::ListNode *pos = Rooms.GetBegin(); pos!=NULL; pos=pos->Next)
	{
		room = pos->Value;
		res = room->GetRoom(point);
		if (res)
		{
			break;		
		}
	}
	return res;
}


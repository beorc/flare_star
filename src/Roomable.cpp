#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\roomable.h"
#include "Room.h"

Roomable::Roomable(void) :
Owner(NULL),
Rooms(100),
RoomOnlyMode(IRoomable::ROM_DESTROY),
StoreInRoom(true)
{
}

Roomable::Roomable(IAAObject *owner) :
Owner(owner),
Rooms(100),
RoomOnlyMode(IRoomable::ROM_DESTROY),
StoreInRoom(true)
{
}

Roomable::~Roomable(void)
{
}

void Roomable::AddRoom(Room* room)
{
	Rooms.PushBack(room);
}

void Roomable::RemoveFromRooms()
{    
	assert(Owner);
	//RoomsPool::iterator iPos = Rooms.begin(), iEnd = Rooms.end();

	for (RoomsPool::ListNode *pos = Rooms.GetBegin();pos!=NULL;pos=pos->Next)
	{
		pos->Value->RemoveObject(Owner);
	}
}

bool Roomable::IsRoomParent(Room* room)
{
	//RoomsPool::iterator iPos = Rooms.begin(), iEnd=Rooms.end();

	//for (;iPos!=iEnd;++iPos)
	for (RoomsPool::ListNode *pos = Rooms.GetBegin();pos!=NULL;pos=pos->Next)
	{
		if (pos->Value==room)
		{                                    
			return true;
		}								
	}
	return false;
}

Room *Roomable::GetRoom(Ogre::Vector3 &point)
{
	//RoomsPool::iterator iPos=Rooms.begin(), iEnd=Rooms.end();

	Room *room=NULL;

	for (RoomsPool::ListNode *pos = Rooms.GetBegin();pos!=NULL;pos=pos->Next)
	{
		if (pos->Value->GetBox()->contains(point))
		{
			room=pos->Value;
			break;
		}
	}
	return room;
}

IRoomable::RoomsPool *Roomable::GetRooms()
{
	return &Rooms;
}

Ogre::AxisAlignedBox Roomable::GetBoundingBox() const
{
	return Box;
}

void Roomable::SetBox(Ogre::AxisAlignedBox box)
{
	Box = box;
}

int Roomable::GetRoomOnly() const
{
	return (int)RoomOnlyMode;
}

void Roomable::SetRoomOnly(int mode)
{
	RoomOnlyMode = (IRoomable::ERoomOnlyMode)mode;
}

bool Roomable::GetStoreInRoom() const
{
    return StoreInRoom;
}

bool Roomable::Parse(TiXmlElement* xml)
{
	if (NULL==xml)
		return false;
	int res, val;
	res=xml->QueryIntAttribute("room_only", &val);
	
	if (TIXML_SUCCESS==res)
		RoomOnlyMode = (IRoomable::ERoomOnlyMode)val;

    res=xml->QueryIntAttribute("room_store", &val);

    if (TIXML_SUCCESS==res)
        StoreInRoom = val ? true : false;

	return true;
}
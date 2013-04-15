#pragma once

#include <Ogre.h>
#include "List.h"

class Room;

class IRoomable
{
public:
	enum ERoomOnlyMode {ROM_NONE, ROM_RESTORE, ROM_DESTROY, ROM_SCRIPT};
	typedef List<Room *> RoomsPool;

    virtual Room *GetRoom(Ogre::Vector3 &point) = 0;    

	virtual void AddRoom(Room* room) = 0;	
	virtual void RemoveFromRooms() = 0;	
	
	virtual RoomsPool *GetRooms() = 0;
	virtual bool IsRoomParent(Room*) = 0;
	virtual Ogre::AxisAlignedBox GetBoundingBox() const = 0;
	virtual int GetRoomOnly() const = 0;
//	virtual void SetRoomOnly(bool) = 0;
    virtual bool GetStoreInRoom() const = 0;
	virtual void SetRoomOnly(int) = 0;
};

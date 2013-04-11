#pragma once
#include <Ogre.h>

#include "IRoomable.h"

class Room;
class IAAObject;

class Roomable : public IRoomable
{
public:	
	Roomable(void);
	Roomable(IAAObject *owner);
	virtual ~Roomable(void);

	void AddRoom(Room* room);	
	void RemoveFromRooms();	
	Room *GetRoom(Ogre::Vector3 &point);
	virtual RoomsPool *GetRooms();
	bool IsRoomParent(Room*);

	virtual void SetBox(Ogre::AxisAlignedBox box);
	virtual Ogre::AxisAlignedBox GetBoundingBox() const;
	virtual int GetRoomOnly() const;
	virtual void SetRoomOnly(int);
	virtual bool Parse(TiXmlElement* xml);
    virtual bool GetStoreInRoom() const;

protected:
	RoomsPool Rooms;
	IAAObject *Owner;
	Ogre::AxisAlignedBox Box;
	IRoomable::ERoomOnlyMode RoomOnlyMode;
    bool StoreInRoom;
};

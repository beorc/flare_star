#pragma once

#include <OgreAxisAlignedBox.h>
#include <OgreRay.h> 

#include "List.h"
#include "Array.h"
using namespace std;

class IAAObject;

typedef Ogre::AxisAlignedBox RoomBoxType;

class Room
{
public:	
    typedef IAAObject ObjectType;
	typedef List<ObjectType*> ObjectsPool;	
	typedef Array<Room> ChildRoomsPool;
	

	Room(void);
	Room(RoomBoxType box);
	~Room(void);

	void Init(RoomBoxType box, unsigned id=0);
	void Divide(unsigned div);
	void	 AddObject(ObjectType* object);
	unsigned	 GetRoomID();
	void RemoveObject(ObjectType* object);
	//void RemoveObject(unsigned InRoomID);
	RoomBoxType *GetBox();
	RoomBoxType *GetBox(unsigned i);

	bool PutObject(ObjectType* object);
	bool UpdateObject(ObjectType* object);	
	bool UpdateObjects();
	void GetEdgeRooms(unsigned id, List<Room*> &OutRooms, Ogre::Ray ray);

	void UpdateDebugInfo();

	ChildRoomsPool *GetChildRooms();
	ObjectsPool *GetChildObjects();

	Room *GetRoom(Ogre::Vector3 point);
	
	void SetParentRoom(Room *);
	Room *GetParentRoom();

private:
	//static unsigned MaxRoomID;
	unsigned RoomID;
	RoomBoxType		Box;
	ChildRoomsPool		Childs;
	ObjectsPool			Objects;
	Room *ParentRoom;

	unsigned SideLength, ChildSideLength, Divider, ChildSquaredSize;

#ifdef ROOMS_SHOW_DEBUG_DATA
	Ogre::SceneNode *DebugNodes[8];
    Ogre::Entity *DebugEntities[8];
#endif
};

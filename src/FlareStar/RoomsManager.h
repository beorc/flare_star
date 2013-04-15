#pragma once
#include "XMLConfigurable.h"
#include "List.h"
#include "Array.h"

class Room;
class IAAObject;
class AACamera;

class RoomsManager : XMLConfigurable
{
public:    
    typedef List<Room*> RoomsPool; 
	typedef Array<IAAObject*> OuterObjectsContainer;
    RoomsManager(void);
    ~RoomsManager(void);

    virtual bool Parse(TiXmlElement* xml);

	RoomsPool *GetRooms();
	void ClearRooms();
	void ClearOuterObjects();
	size_t GetOuterObjectsNumber();
	OuterObjectsContainer *GetOuterObjects();	
	bool PutObject(IAAObject* object);
	bool UpdateObject(IAAObject* object);
    		
	bool IsValidPosition(Ogre::Vector3);
	Room *GetRoom(const Ogre::Vector3 &point);
	
protected:    
    RoomsPool	Rooms;
	OuterObjectsContainer OuterObjects;
	size_t OuterObjectsNumber;
	void AddOuterObject(IAAObject *);
};

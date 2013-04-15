#pragma once
#include "AIPathFinderStrategy.h"

class IAAObject;
class Room;

class AIRayPathFinderStrategy :	public AIPathFinderStrategy
{
public:	
	AIRayPathFinderStrategy(void);
	~AIRayPathFinderStrategy(void);

	virtual void Step(unsigned timeMs);

	void Route(Ogre::Vector3 point);
	void GoTo(Ogre::Vector3 point);
	Vector3 RouteToTarget();
	void CalcLines(Ogre::Ray &line);
	virtual bool Parse(TiXmlElement* xml);
	virtual void Init();
	
protected:
	struct ObjZBuffer {
		IAAObject *object;
		double			  distance;
	};

	Vector3 CashedTargetPoint, LastTargetPoint;
			
	bool HaveCashedTarget, HaveTarget;
	bool GetIntersectionObjects(Ogre::Ray line, List<Room*> &EdgeRooms, List<ObjZBuffer> *ObjBuffer);
	bool IsAccessible(Ogre::Ray line, List<Room*> &EdgeRooms);

	//Ogre::Entity *DebugLinesNodes[9];
	Ogre::Ray *Lines;
	Ogre::Vector3 LineOrigins[9];

	unsigned LastChangingDirectionTime;
	IAAObject *ClosestObj;	
	int	LinesNumber;
	bool Inited;
	int WaitTargetTimeout;
};

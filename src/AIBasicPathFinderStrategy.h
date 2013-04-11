#pragma once
#include "AIPathFinderStrategy.h"

class IAAObject;
class Room;

class AIBasicPathFinderStrategy :	public AIPathFinderStrategy
{
public:	
	AIBasicPathFinderStrategy(void);
	~AIBasicPathFinderStrategy(void);

	virtual void Step(unsigned timeMs);
protected:
};

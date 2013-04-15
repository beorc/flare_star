#pragma once

#include "XMLConfigurable.h"
#include "AAObject.h"

#include <Ogre.h>

class Incubator : public AAObject
{
public:	
	enum {FireNumberNone=-100000};
	struct SDescription
	{
		int x,y,z,period, dir_x, dir_y, dir_z;
		TiXmlElement*	Content;
		int FireNumber;
	};
	typedef List<SDescription*> DescriptionsContainer;
	Incubator();
	~Incubator(void);

	
	void Step(unsigned timeMs);
	void SetPosition(Ogre::Vector3 pos);
	virtual Ogre::Vector3 GetPosition() const;

	virtual bool Parse(TiXmlElement* xml);	
	
private:
	int Period;
	std::string XMLConfigname;
	Ogre::Vector3 Position, Direction;
    int RandX, RandY, RandZ;

	int LastTimeStamp, BornTimeout, LastBornTime;

    //std::auto_ptr<TiXmlNode> Description;	
	DescriptionsContainer Descriptions;
	//DescriptionsContainer::ListNode *PendingNode;
	DescriptionsContainer PendingDescriptions;
	int FireNumber;

	void CreateObject(SDescription *descr);
};

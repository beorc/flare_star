#pragma once
#include "Trigger.h"
#include "SceneObject.h"

class Container : public Trigger, public SceneObject
{
public:	
	OBJDESTROY_DEFINITION
	Container(void);
	virtual ~Container(void);

	virtual void Step( unsigned timeMs );
	virtual bool Parse(TiXmlElement* xml);
    virtual bool Init();

	virtual bool ApplyIn(IAAObject*);		
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed) const;
	
protected:
	 std::auto_ptr<TiXmlNode> Content;	 	 
};

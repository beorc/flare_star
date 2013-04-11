#pragma once

#include "ISubEffect.h"

class BaseSubEffect : public ISubEffect
{
public:	
	BaseSubEffect(void);
	virtual ~BaseSubEffect(void);

	virtual bool Parse(TiXmlElement* xml);
	virtual void Step(unsigned timeMs);
	virtual bool Start();
    virtual bool Stop();
	
	ISubEffect::ESubEffectType GetType();
	void SetType(ISubEffect::ESubEffectType);

	virtual void SetRelativePosition(Ogre::Vector3 pos);
    virtual void SetRelativeOrientation(Ogre::Quaternion or);

	virtual void SetDuration(int duration);
	virtual int GetLID() const;
	virtual void SetLID(int lid);

	virtual bool Init();

    virtual void SetMaster(IMasterEffect *master);

protected:
	IMasterEffect *Master;
		
	Ogre::Vector3 RelativePos;
	int Duration;
    ISubEffect::ESubEffectType Type;	
    Ogre::Quaternion RelOrientation;
	unsigned LastTime;
	int LID;
};

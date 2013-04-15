#pragma once

class IMasterEffect;

class ISubEffect
{
public:    
	enum ESubEffectType {SET_NONE,SET_MESH,SET_BILLBOARD,SET_PARTICLE};
	
	virtual bool Parse(TiXmlElement* xml) = 0;
	virtual bool Init() = 0;
	virtual void Step(unsigned timeMs) = 0;
	virtual bool Start() = 0;
	virtual bool Stop() = 0;
	
	virtual ESubEffectType GetType() = 0;
	virtual void SetType(ESubEffectType) = 0;
			
	virtual int GetLID() const = 0;
	virtual void SetLID(int lid) = 0;

	virtual void Destroy() = 0;

    virtual void SetMaster(IMasterEffect *parent) = 0;
};

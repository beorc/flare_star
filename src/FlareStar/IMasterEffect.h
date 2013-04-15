#pragma once

class IScenable;
class TiXmlElement;

namespace Ogre
{
    class SceneNode;
}

class IMasterEffect
{
public:    
	enum EMasterEffectType {MET_NONE,MET_ONSHOOT,MET_ONBLOW,MET_PERMANENT};
	
	virtual bool Parse(TiXmlElement* xml) = 0;
	virtual bool Init() = 0;
	virtual void Step(unsigned timeMs) = 0;
	virtual bool Start() = 0;
	virtual bool Stop() = 0;

	virtual const char *GetName() = 0;
	virtual EMasterEffectType GetType() = 0;
	virtual void SetType(EMasterEffectType) = 0;
	
	virtual void SetParent(IScenable *parent) = 0;
	virtual void SetParentNode(Ogre::SceneNode *parent_node) = 0;

    virtual IScenable *GetParent() const = 0;
    virtual Ogre::SceneNode *GetEffectNode() const = 0;
	
	virtual void SetDuration(int duration) = 0;
	virtual int GetLID() const = 0;
	virtual void SetLID(int lid) = 0;

	virtual void Destroy() = 0;
};

#pragma once

#include "CommonDeclarations.h"
#include "IMasterEffect.h"
#include "List.h"
#include <vector>
#include <list>

namespace OgreAL {
    class Sound;
}

class ISubEffect;

class MasterEffect : public IMasterEffect
{
public:
	OBJDESTROY_DEFINITION
	struct SSoundDescription
	{
		int QueueNumber, Priority;
	};

    typedef std::vector<OgreAL::Sound *> SoundsContainer;
    typedef std::vector<ISubEffect*> SubEffectsContainer;

	MasterEffect(void);
	virtual ~MasterEffect(void);

	virtual bool Parse(TiXmlElement* xml);
	virtual void Step(unsigned timeMs);
	virtual bool Start();
    virtual bool Stop();

	const char *GetName();
	IMasterEffect::EMasterEffectType GetType();
	void SetType(IMasterEffect::EMasterEffectType);

	virtual void SetRelativePosition(Ogre::Vector3 pos);
    virtual void SetRelativeOrientation(Ogre::Quaternion or);

	virtual void SetParent(IScenable *parent);
	virtual void SetParentNode(Ogre::SceneNode *parent_node);

	virtual void SetDuration(int duration);
	virtual int GetLID() const;
	virtual void SetLID(int lid);

	virtual bool Init();
	bool InitSounds();

    virtual IScenable *GetParent() const;    
	virtual Ogre::SceneNode *GetEffectNode() const;

protected:
	IScenable *Parent;
	Ogre::SceneNode *ParentNode, *EffectNode;
	char *Name;
	Ogre::Vector3 RelativePos;
	int Duration;
    IMasterEffect::EMasterEffectType Type;
	bool Running, RunOnCreate;	
    Ogre::Quaternion RelOrientation;	
	int LID;
    SubEffectsContainer SubEffects;
    bool IsLoop;
    unsigned LastTime;

    std::auto_ptr<SoundsContainer> Sounds;
    std::auto_ptr< List<char*> > SoundFiles;
	std::auto_ptr< std::list<SSoundDescription> > SoundQueues;
};

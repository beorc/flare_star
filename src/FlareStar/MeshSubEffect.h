#pragma once
#include "BaseSubEffect.h"
#include "CommonDeclarations.h"

class MeshSubEffect : public BaseSubEffect
{
public:
    OBJDESTROY_DEFINITION

    MeshSubEffect(void);
    virtual ~MeshSubEffect(void);

    virtual bool Parse(TiXmlElement* xml);    
    //virtual void Step(unsigned timeMs);
    virtual bool Start();
    virtual bool Stop();
    bool InitEntity();
    virtual bool Init();
protected:	
    int Size;
    Ogre::MovableObject *OgreEntity;
    char *Modelname;
	int OgreRenderQueue;
};

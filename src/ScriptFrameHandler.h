#pragma once

#include "IFrameHandler.h"

class TiXmlElement;

#include "Array.h"

class ScriptFrameHandler : public IFrameHandler
{
public:
    ScriptFrameHandler(void);
    ~ScriptFrameHandler(void);

    virtual void Step(unsigned timeMs);
    virtual bool Parse(TiXmlElement *xml);
	virtual void Close();

	virtual bool MouseMoved( const OIS::MouseEvent &arg );
	virtual bool KeyReleased( const OIS::KeyEvent &arg );
	virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id );
protected:
    char ScriptValues[10];    
    char *ScriptHandlers[3];   
};

#pragma once

#include "IFrameHandler.h"

class TiXmlElement;

#include "ScriptFrameHandler.h"
#include "Array.h"

class CombatFrameHandler : public ScriptFrameHandler
{
public:	
    CombatFrameHandler(void);
    ~CombatFrameHandler(void);

	void Init();
	void InitObjects();
	void ConfigureCells(TiXmlElement *xml);
    void Step(unsigned timeMs);
    bool Parse(TiXmlElement *xml);
	void Close();

	void Start();
	void Stop();

	void Exit();

	virtual bool KeyPressed( const OIS::KeyEvent &arg );
	virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id );

protected:
	bool ConfigureGlobal(TiXmlElement *xml);

	bool ExitRequested;
		
	Ogre::Vector3 CameraPosition;
	Ogre::SceneNode *CameraNode;
};

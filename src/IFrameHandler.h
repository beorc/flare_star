#pragma once
#define OIS_DYNAMIC_LIB
#include <OIS.h>

class IFrameHandler
{
public:
	virtual void Step(unsigned timeMs) = 0;
	virtual bool KeyPressed( const OIS::KeyEvent &arg ) = 0;
	virtual bool KeyReleased( const OIS::KeyEvent &arg ) = 0;
	virtual bool MousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) = 0;
	virtual bool MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id ) = 0;
	virtual bool MouseMoved( const OIS::MouseEvent &arg ) = 0;
};

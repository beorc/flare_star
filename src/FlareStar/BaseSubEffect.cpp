#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "BaseSubEffect.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "ObjectsLibrary.h"

BaseSubEffect::BaseSubEffect(void) :
RelativePos(0,0,0),
Duration(-1),
Type(ISubEffect::SET_NONE),
RelOrientation(Ogre::Quaternion::IDENTITY),
LastTime(0),
LID(0),
Master(NULL)
{
}

BaseSubEffect::~BaseSubEffect(void)
{
}

bool BaseSubEffect::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened
	
	int x=0, y=0, z=0, resx, resy, resz;
	resx = xml->QueryIntAttribute("x", &x);
	resy = xml->QueryIntAttribute("y", &y);
	resz = xml->QueryIntAttribute("z", &z);

	if (TIXML_SUCCESS==resx && TIXML_SUCCESS==resy && TIXML_SUCCESS==resz) 
	{
		RelativePos = Ogre::Vector3((float)x,(float)y,(float)z);
	}

	float dirx, diry, dirz;
	resx = xml->QueryFloatAttribute("dirx", &dirx);
	resy = xml->QueryFloatAttribute("diry", &diry);
	resz = xml->QueryFloatAttribute("dirz", &dirz);

	if (TIXML_SUCCESS==resx && TIXML_SUCCESS==resy && TIXML_SUCCESS==resz)
	{
		Ogre::Vector3 relative_dir(dirx,diry,dirz);
		RelOrientation = relative_dir.getRotationTo(Ogre::Vector3::UNIT_Z);
	}
	
    int res;
	res = xml->QueryIntAttribute("duration", &Duration);
		
	return true;
}

void BaseSubEffect::Step(unsigned timeMs)
{    
    return;
}

bool BaseSubEffect::Start()
{
	return false;
}

bool BaseSubEffect::Stop()
{
    LastTime=0;

    return false;
}

ISubEffect::ESubEffectType BaseSubEffect::GetType()
{
	return Type;
}

void BaseSubEffect::SetType(ISubEffect::ESubEffectType type)
{
	Type=type;
}

void BaseSubEffect::SetRelativePosition(Ogre::Vector3 pos)
{
	RelativePos = pos;	
}

void BaseSubEffect::SetRelativeOrientation(Ogre::Quaternion or)
{
    RelOrientation = or;
}

void BaseSubEffect::SetDuration(int duration)
{
	Duration = duration;
}

int BaseSubEffect::GetLID() const
{
	return LID;
}

void BaseSubEffect::SetLID(int lid)
{
	LID = lid;
}

bool BaseSubEffect::Init()
{
	return true;
}

void BaseSubEffect::SetMaster(IMasterEffect *master)
{
    Master = master;
}


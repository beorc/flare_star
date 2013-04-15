#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\aistrategy.h"

AIStrategy::AIStrategy(STRATEGY_TYPE type) :
Type(type),
TargetID(-1),
Owner(NULL),
Parent(NULL)
{
}

AIStrategy::~AIStrategy(void)
{
}

void AIStrategy::SetParent(IAAObject* parent)
{
	Parent = parent;
}

AIStrategy::STRATEGY_TYPE AIStrategy::GetType() const
{
	return Type;
}

void AIStrategy::Init()
{
}

void AIStrategy::SetOwner(IAI* owner)
{
	Owner = owner;
}

void AIStrategy::SetTargetID(int id)
{
	TargetID = id;
}

int AIStrategy::GetTargetID() const
{
	return TargetID;
}

bool AIStrategy::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	//const char *str;	

	// parse the file until end reached

	//str = xml->Value();

	int res,val;

	res = xml->QueryIntAttribute("target_id", &val);
	if (TIXML_SUCCESS==res)
		SetTargetID(val);

	return TIXML_SUCCESS==res;
}

IAAObject* AIStrategy::GetParent()
{
    return Parent;
}


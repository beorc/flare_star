#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\equipment.h"
//#include "Core.h"
#include "CommonDeclarations.h"


Equipment::Equipment(EQUIPMENT_TYPE type) :
SceneObject(),
Type(type),
Owner(NULL)
{
}

Equipment::~Equipment(void)
{
}

void Equipment::UpdateBox()
{
}

void Equipment::SetOwner(IEquipped *owner)
{
	Owner = owner;
}

Vector3 Equipment::GetRelativePos()
{
	return RelativePos;
}

void Equipment::SetRelativePos(Vector3 pos)
{		
	RelativePos = pos;	
}

void Equipment::SetParent(IScenable *parent)
{	
	SceneObject::SetParent(parent);
	Parent = parent;
		
	/*Node->setPosition(parent->GetInitialRotation().Inverse()*RelativePos);
	Node->rotate(parent->GetInitialRotation().Inverse());*/	
}

bool Equipment::Init()
{   
	bool res = SceneObject::Init();
	Node->setPosition(Parent->GetInitialRotation().Inverse()*RelativePos);
	Node->rotate(Parent->GetInitialRotation().Inverse()*InitialRotation);
	return res;
}

Equipment::EQUIPMENT_TYPE Equipment::GetType()
{
	return Type;
}

int Equipment::GetSlotType()
{
	return SlotType;
}

bool Equipment::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	
	str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}

	SceneObject::Parse(xml);

	str = xml->Value();
	assert(strcmp(str,"weapon")==0);	
	
	//const char *modelname = xml->Attribute("modelname");
	//SetModel(modelname);

	int res;
	res = xml->QueryIntAttribute("slottype", &SlotType);	
	assert(TIXML_SUCCESS==res);

	return true;
}



#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Trigger.h"
#include "Room.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "GenericWeapon.h"
#include "RoomsManager.h"
#include "Roomable.h"
#include "ScriptManager.h"

Trigger::Trigger(void) :
BufferZoneSize(0),
TriggersPosition(Ogre::Vector3::ZERO)
{	
	memset(TriggersScriptHandlers,0,sizeof(TriggersScriptHandlers));
}

Trigger::~Trigger(void)
{
	size_t count = sizeof(TriggersScriptHandlers)/sizeof(char*);
	for (size_t i=0;i<count;++i)
	{
		delete [] TriggersScriptHandlers[i];	
	}
}

bool Trigger::Init()
{   
	AddControllingObject(CommonDeclarations::GetIDObject(1));
    return true;
}

void Trigger::Step( unsigned timeMs )
{	
    //LastTime+=timeMs;
	//if (LastTime<5)
	//	return;
    //LastTime=0;

    //Ogre::Vector3 pos = TriggersPosition;
	IAAObject *object;
	Ogre::AxisAlignedBox box_orig = GetBoundingBox(true), box_min, box_max;
	Ogre::Vector3 min=box_orig.getMinimum(), max=box_orig.getMaximum();
	box_min.setExtents(min.x+BufferZoneSize, min.y+BufferZoneSize, min.z+BufferZoneSize,
					   max.x-BufferZoneSize, max.y-BufferZoneSize, max.z-BufferZoneSize);
	box_max.setExtents(min.x-BufferZoneSize, min.y-BufferZoneSize, min.z-BufferZoneSize,
					   max.x+BufferZoneSize, max.y+BufferZoneSize, max.z+BufferZoneSize);
	Ogre::AxisAlignedBox *box;
	
	for (ObjectsContainer::ListNode *pos = ControllingObjects.GetBegin();pos!=NULL;pos=pos->Next)
	{
		object = pos->Value.Object;

		if (pos->Value.Triggering)
			box=&box_max;
		else
			box=&box_min;
		
		IScenable *scen = object->GetScenable();
		Ogre::AxisAlignedBox obj_box = object->GetBoundingBox(true);
		if (scen && obj_box.intersects(*box)) 
		{				
			if (!pos->Value.Triggering)
			{
				ApplyIn(object);
				pos->Value.Triggering=true;
			}	
		} else
		{
			if (pos->Value.Triggering)
			{
				ApplyOut(object);
				pos->Value.Triggering=false;
			}
		}
	}
}

bool Trigger::ApplyIn(IAAObject* obj)
{	
	bool applyed=false;
	
	if (TriggersScriptHandlers[0])
		ScriptManager::GetInstance()->Call(TriggersScriptHandlers[0], "i", false, 0);
		 
	return applyed;
}

bool Trigger::ApplyOut(IAAObject* obj)
{	
	bool applyed=false;
	
	if (TriggersScriptHandlers[1])
		ScriptManager::GetInstance()->Call(TriggersScriptHandlers[1], "i", false, 1);

	return applyed;
}

bool Trigger::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened


	const char *str, *cfg;    

	str = xml->Value();

	cfg = xml->Attribute("cfg");

	if (cfg)
	{
		return XMLConfigurable::Parse(cfg);
	}
	
    int res, min_x, min_y, min_z, max_x, max_y, max_z, val;

	if (strcmp(str,"trigger")==0) 
	{	
        res = xml->QueryIntAttribute("min_x", &min_x);
        assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("min_y", &min_y);
		assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("min_z", &min_z);
		assert(TIXML_SUCCESS==res);

		res = xml->QueryIntAttribute("max_x", &max_x);
		assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("max_y", &max_y);
		assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("max_z", &max_z);
		assert(TIXML_SUCCESS==res);

		TriggersBox.setExtents(min_x, min_y, min_z, max_x, max_y, max_z);
	}

	res = xml->QueryIntAttribute("buffer_zone", &val);
	if (TIXML_SUCCESS==res)
		BufferZoneSize = (unsigned)val;
		
	TiXmlElement *xml_element = 0;
	const char *handler;	
	xml_element = xml->FirstChildElement("scripting");	
	if (xml_element)
	{
		xml_element = xml_element->FirstChildElement("handler");	
		while (xml_element)
		{
			handler = xml_element->Attribute("function");
			str=xml_element->Attribute("action");
			if (strcmp(str,"apply_in")==0) 
			{
				TriggersScriptHandlers[0]=AAUtilities::StringCopy(handler);				
			}
			if (strcmp(str,"apply_out")==0) 
			{
				TriggersScriptHandlers[1]=AAUtilities::StringCopy(handler);				
			}
			xml_element = xml_element->NextSiblingElement("handler");
		}		
	}

	return true;
}

Ogre::AxisAlignedBox Trigger::GetBoundingBox(bool transformed) const
{	
    if (!transformed)
    {
        //Ogre::AxisAlignedBox box(-Size,-Size,-Size,Size,Size,Size);
        return TriggersBox;
    } else
    {        
        Ogre::AxisAlignedBox box(TriggersPosition+TriggersBox.getMinimum(),TriggersPosition+TriggersBox.getMaximum());
		
        return box;
    }	
}

void Trigger::SetTriggersPosition(Ogre::Vector3 pos)
{
	TriggersPosition = pos;
}

void Trigger::AddControllingObject(IAAObject *obj)
{
	STriggeredObject tobj;
	tobj.Object = CommonDeclarations::GetIDObject(1);
	tobj.Triggering = false;
	ControllingObjects.PushBack(tobj);
}





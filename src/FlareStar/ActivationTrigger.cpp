#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "ActivationTrigger.h"
#include "Room.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "RoomsManager.h"
#include "Roomable.h"
#include "Core.h"
#include "Debugging.h"

#include <vector>

//#define ACTIVATION_DEBUG

ActivationTrigger::ActivationTrigger(void) : 
Trigger()
{
	Tag = T_TRIGGER;
}

ActivationTrigger::~ActivationTrigger(void)
{	
}

void ActivationTrigger::Register()
{
	AAObject::Register();
	Init();
}

bool ActivationTrigger::Init()
{
	Trigger::Init();
	
	Ogre::AxisAlignedBox box = ActivationBox, *room_box;

	{
		List<Room*> rooms;		
		RoomsManager::RoomsPool *pool = CommonDeclarations::GetRoomManager()->GetRooms();
		for (RoomsManager::RoomsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
		{
			room_box = pos->Value->GetBox();
			if (box.contains(room_box->getCenter())) 
			{
				rooms.PushBack(pos->Value);
			}
		}
		ActivationRooms.Resize(rooms.GetSize());
		size_t i=0;
		for (RoomsManager::RoomsPool::ListNode *pos = rooms.GetBegin(); pos!=NULL; pos=pos->Next, ++i)
		{
			ActivationRooms.Data[i] = pos->Value;
		}
	}	

	{
		List<IAAObject*> objects;	
		CommonDeclarations::ObjectsPool *pool = CommonDeclarations::GetObjects();
		for (CommonDeclarations::ObjectsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
		{
			if (pos->Value->IsVolatile() && box.contains(pos->Value->GetPosition())) 
			{
				objects.PushBack(pos->Value);
			}
		}
		ActivationObjects.Resize(objects.GetSize());
		//std::vector<IAAObject*>::iterator iPos = objects.begin(), iEnd = objects.end();
		//for (size_t i=0;iPos!=iEnd;++iPos,++i)
		size_t i=0;
		for (CommonDeclarations::ObjectsPool::ListNode *pos = objects.GetBegin(); pos!=NULL; pos=pos->Next, ++i)
		{
			ActivationObjects.Data[i] = pos->Value;
		}
	}

    return true;
}

void ActivationTrigger::Step( unsigned timeMs )
{
	if (Active)
		Trigger::Step(timeMs);	
}

bool ActivationTrigger::ApplyIn(IAAObject* obj)
{		
	if (obj)
		Trigger::ApplyIn(obj);
	bool applyed=true, found=false;

#ifdef ACTIVATION_DEBUG
	char log[100];
	sprintf(log,"in %d\n", ID);
	Debugging::Log("activation",log);
#endif

	{
		RoomsManager::RoomsPool *pool = CommonDeclarations::GetRoomManager()->GetRooms();
		for (size_t i=0; i<ActivationRooms.Size; ++i)
		{
			found=false;
			for (RoomsManager::RoomsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
			{
				if (ActivationRooms.Data[i]==pos->Value)
				{
					found=true;
					break;
				}
			}
			if (!found)
				pool->PushBack(ActivationRooms.Data[i]);
		}
	}

	{
		//Core::ObjectsPool *pool = Core::GetInstance()->GetObjects();		
		Core *inst = Core::GetInstance();
		for (size_t i=0; i<ActivationObjects.Size; ++i)
		{
			if (ActivationObjects.Data[i])
				inst->IncludeObjectToQueue(ActivationObjects.Data[i]);
			/*found=false;
			for (Core::ObjectsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
			{
				if (ActivationObjects.Data[i]==pos->Value)
				{
					found=true;
					break;
				}
			}
			if (!found)
				pool->PushBack(ActivationObjects.Data[i]);*/
		}
	}
	
	return applyed;
}

bool ActivationTrigger::ApplyOut(IAAObject* obj)
{
	if (obj)
		Trigger::ApplyOut(obj);
	bool applyed=true, found=false;

	#ifdef ACTIVATION_DEBUG
	char log[100];
	sprintf(log,"out %d\n", ID);
	Debugging::Log("activation",log);
	#endif

	{
		RoomsManager::RoomsPool *pool = CommonDeclarations::GetRoomManager()->GetRooms();
		for (size_t i=0; i<ActivationRooms.Size; ++i)
		{
			found=false;
			for (RoomsManager::RoomsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
			{
				if (ActivationRooms.Data[i]==pos->Value)
				{
					found=true;
					break;
				}
			}
			if (found)
				pool->Remove(ActivationRooms.Data[i]);
		}
	}	

	{
		//Core::ObjectsPool *pool = Core::GetInstance()->GetObjects();		
		Core *inst = Core::GetInstance();
		for (size_t i=0; i<ActivationObjects.Size; ++i)
		{
			if (ActivationObjects.Data[i])
				inst->ExcludeObjectFromQueue(ActivationObjects.Data[i]);
			/*found=false;
			for (Core::ObjectsPool::ListNode *pos = pool->GetBegin(); pos!=NULL; pos=pos->Next)
			{
				if (ActivationObjects.Data[i]==pos->Value)
				{
					found=true;
					break;
				}
			}
			if (found)
				pool->Remove(ActivationObjects.Data[i]);*/
		}
	}

	return applyed;
}

bool ActivationTrigger::Parse(TiXmlElement* xml)
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

	Trigger::Parse(xml);
	AAObject::Parse(xml);
    
	int res, min_x, min_y, min_z, max_x, max_y, max_z;
	if (strcmp(str,"trigger")==0) 
	{
		TiXmlElement *xml_element = 0;
		xml_element = xml->FirstChildElement("activation_box");
		assert(xml_element);
		
		if (xml_element)
		{
			res = xml_element->QueryIntAttribute("min_x", &min_x);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("min_y", &min_y);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("min_z", &min_z);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryIntAttribute("max_x", &max_x);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("max_y", &max_y);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("max_z", &max_z);
			assert(TIXML_SUCCESS==res);
		}		

		ActivationBox.setExtents(min_x, min_y, min_z, max_x, max_y, max_z);
	}

	return true;
}

Ogre::Vector3 ActivationTrigger::GetPosition() const
{
	return TriggersPosition;
}

void ActivationTrigger::SetActive(bool on)
{
	AAObject::SetActive(on);
	//ApplyOut(NULL);
}

Ogre::AxisAlignedBox ActivationTrigger::GetBoundingBox(bool transformed) const
{
	return Trigger::GetBoundingBox(transformed);
}

bool ActivationTrigger::RemoveFromActivationObjects(IAAObject *obj)
{	
	IAAObject *curr_object = NULL;
	for (size_t i=0; i<ActivationObjects.Size; ++i)
	{
		curr_object = ActivationObjects.Data[i];
		if (curr_object==obj)
		{
			ActivationObjects.Data[i] = NULL;
			return true;
		}
	}
	return false;
}

const Array<IAAObject*> *ActivationTrigger::GetActivationObjects() const
{
	return &ActivationObjects;
}

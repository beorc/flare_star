#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\aaobject.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include <tinyxml.h>
#include "Utilities.h"

AAObject::AAObject():
Removing(false),
ID(0),
Registered(false),
Tag(T_UNKNOWN),
LifeTime(0),
Volatile(false),
Active(true),
Group(IAAObject::FA_NONE),
LID(0),
LifeTimeMode(false),
Editable(false)
{
	memset(ScriptValues,0,sizeof(ScriptValues));
	memset(ScriptHandlers,0,sizeof(ScriptHandlers));
}

AAObject::~AAObject(void)
{
	size_t count = sizeof(ScriptHandlers)/sizeof(char*);
	for (size_t i=0;i<count;++i)
		delete [] ScriptHandlers[i];
}

bool AAObject::IsEditable() const
{
	return Editable;
}

void AAObject::SetEditable(bool on)
{
	Editable = on;
}

void AAObject::Register()
{
	/*if (ID!=0)
		CommonDeclarations::AddIDObject(this);*/
	if (ScriptHandlers[0] && ID>0)
		ScriptManager::GetInstance()->Call(ScriptHandlers[0], "i", false, ID);
	/*else
	if (ID>0)
		ScriptManager::GetInstance()->Call("OnRegister", "i", false, ID);*/
	Registered = true;
}

void AAObject::Unregister()
{	
	//if (ScriptHandlers[1] && Active && ID>0)
	//	ScriptManager::GetInstance()->Call(ScriptHandlers[1], "i", false, ID);
	Registered = false;
}

bool AAObject::RestoreBackupPosition()
{
	return false;
}

bool AAObject::Killed()
{	
	if (ScriptHandlers[2] && Active && ID>0)
		ScriptManager::GetInstance()->Call(ScriptHandlers[2], "i", false, ID);
	return true;
}

bool AAObject::IsRemoving()
{
	return Removing;
}

void AAObject::SetRemoving(bool on)
{
	Removing=on;
}

void AAObject::Step(unsigned timeMs)
{
	if (LifeTimeMode)
	{
		LifeTime-=timeMs;
		if (LifeTime<0)
			CommonDeclarations::DeleteObjectRequest(this);
	}
}

void AAObject::SetID(int id)
{
    ID=id;
}

int AAObject::GetID()
{
    return ID;
}

bool AAObject::Parse(TiXmlElement* xml)
{
	    
    if (xml == 0)
        return false; // file could not be opened
		
	xml = XMLConfigurable::CheckLID(xml);
			
    xml->QueryIntAttribute("id",&ID);

	if (ID!=0)
	    CommonDeclarations::AddIDObject(this);

	const char *str;
	int res, val;
	res = xml->QueryIntAttribute("lifetime", &val);

	if (TIXML_SUCCESS == res)
	{
		LifeTimeMode = true;
		LifeTime = val;
	}	
	
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
			if (strcmp(str,"register")==0) 
			{
				ScriptHandlers[0]=AAUtilities::StringCopy(handler);				
			}
			if (strcmp(str,"unregister")==0) 
			{
				ScriptHandlers[1]=AAUtilities::StringCopy(handler);				
			}
			if (strcmp(str,"killed")==0) 
			{
				ScriptHandlers[2]=AAUtilities::StringCopy(handler);				
			}
			xml_element = xml_element->NextSiblingElement("handler");
		}		
	}

	str = xml->Attribute("volatile");
	if (str && strcmp(str,"on")==0)
	{
		Volatile=true;
	}
	
	str = xml->Attribute("active");
	if (str && strcmp(str,"off")==0)		
		Active=false;

	str = xml->Attribute("group");
	if (str)
	{
		if (strcmp(str,"friend")==0)
			Group = IAAObject::FA_FRIEND;
		else
			if (strcmp(str,"alien")==0)
				Group = IAAObject::FA_ALIEN;
	}

    return true;
}

void AAObject::SetScriptValue(int index, char val)
{
	ScriptValues[index] = val;
}

char AAObject::GetScriptValue(int index)
{
	return ScriptValues[index];
}

void AAObject::QueryDestroy()
{
	CommonDeclarations::DeleteObjectRequest(this);
}

//void AAObject::Destroy()
//{
//	Poolable::Destroy();
//}

IPhysical *AAObject::GetPhysical() const
{
    return NULL;
}

IRoomable *AAObject::GetRoomable() const
{
    return NULL;
}

IScriptable *AAObject::GetScriptable() const
{
    return (IScriptable*)this;
}

IScenable *AAObject::GetScenable() const
{
    return NULL;
}

IAI *AAObject::GetAI() const
{
    return NULL;
}

ICollidable *AAObject::GetCollidable() const
{
	return NULL;
}

bool AAObject::GetRegistered()
{
    return Registered;
}

IEquipped *AAObject::GetEquipped() const
{
	return NULL;
}

IDestructable *AAObject::GetDestructable() const
{
	return NULL;
}

void AAObject::SaveTo(TiXmlElement * root) const
{
	 root->SetAttribute("id", ID);
}

//void* AAObject::operator new (size_t size)
//{
//	return malloc(size);
//}
//
//void AAObject::operator delete (void* ptr)
//{	
//	//implicit destructor call
//	return free(ptr);
//}
//
//void* AAObject::operator new (size_t size, MemAllocator &allocator)
//{
//	return allocator.Alloc();
//}
//
//void AAObject::operator delete (void* ptr, MemAllocator &allocator)
//{	
//	//implicit destructor call
//	return allocator.Free(ptr);
//}

int AAObject::GetTag() const
{
	return Tag;
}

void AAObject::SetTag(int tag)
{
	Tag = tag;
}

bool AAObject::IsVolatile() const
{
	return Volatile;
}

void AAObject::SetActive(bool on)
{
	Active=on;
}

bool AAObject::GetActive() const
{
	return Active;
}

int AAObject::GetGroup() const
{
	return (int)Group;
}

void AAObject::SetGroup(int group)
{
	Group = (EGroup)group;
}

int AAObject::GetLID() const
{
	return LID;
}
void AAObject::SetLID(int lid)
{
	LID = lid;
}

int AAObject::GetDescriptableID() const
{
	return 0;
}

Ogre::AxisAlignedBox AAObject::GetBoundingBox(bool transformed) const
{
	Ogre::AxisAlignedBox box(-1,-1,-1,1,1,1);
	return box;
}

void AAObject::SetPosition(Ogre::Vector3)
{
}

void AAObject::SetAI(IAI *)
{
}

BonusesKeeper *AAObject::GetBonusesKeeper()
{
	return NULL;
}

ICombatable *AAObject::GetCombatable() const
{
	return NULL;
}


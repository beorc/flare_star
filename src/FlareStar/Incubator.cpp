#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Incubator.h"
#include "SceneObject.h"
#include "CommonDeclarations.h"
#include "Debugging.h"

#include "ScriptManager.h"
#include "Player.h"
#include "ObjectsLibrary.h"

Incubator::Incubator():
AAObject(),
LastTimeStamp(-900000),
Period(0),
Position(Ogre::Vector3::ZERO),
Direction(Ogre::Vector3::UNIT_Z),
RandX(0),
RandY(0),
RandZ(0),
BornTimeout(0),
LastBornTime(0),
FireNumber(Incubator::FireNumberNone)
{
	Tag=T_INCUBATOR;
}

Incubator::~Incubator(void)
{
	/*if (Description)
		delete Description;*/
	for (DescriptionsContainer::ListNode *pos = Descriptions.GetBegin();pos!=NULL;pos=pos->Next)
	{
		delete pos->Value->Content;
		delete pos->Value;
	}
	Descriptions.Clear();
}

void Incubator::SetPosition(Ogre::Vector3 pos)
{
	Position=pos;
}

void Incubator::Step(unsigned timeMs)
{
    LastTimeStamp+=timeMs;
	
	if (BornTimeout>0)
	{
		LastBornTime+=timeMs;
		if (LastBornTime>=BornTimeout && !PendingDescriptions.IsEmpty())
		{
			DescriptionsContainer::ListNode *first = PendingDescriptions.GetBegin();
			CreateObject(first->Value);
			PendingDescriptions.PopFront();
			LastBornTime = 0;

			if (PendingDescriptions.IsEmpty())
			{
				if (FireNumber!=Incubator::FireNumberNone && FireNumber<=0)
				{
					CommonDeclarations::DeleteObjectRequest(this);
				}
				if (Period==0)
				{
					Period=-1;
					CommonDeclarations::DeleteObjectRequest(this);
				}
			}			

			return;
		}
	}	
    
	if (Active && Period>=0 && (LastTimeStamp<0 || LastTimeStamp>=Period) && PendingDescriptions.IsEmpty())
	{
		if (FireNumber!=Incubator::FireNumberNone)
			--FireNumber;
		TiXmlElement *xml_element = 0;
		SDescription *descr;
		for (DescriptionsContainer::ListNode *pos = Descriptions.GetBegin();pos!=NULL;pos=pos->Next)
		{
			descr = pos->Value;
			xml_element = descr->Content;

			int res, val;
			bool period_passed=false;
			
			
			if (descr->period>0)
			{
				val=0;
				res = xml_element->QueryIntAttribute("_pe", &val);
				
				val+=Period;
				if (val>=descr->period)
				{
					val=0;	
					period_passed=true;
				}
				xml_element->SetAttribute("_pe",val);
				if (!period_passed)
				{
					//xml_element = xml_element->NextSiblingElement("obj");
					continue;
				}
			}

			if (BornTimeout>0)
				PendingDescriptions.PushBack(descr);
			else
				CreateObject(descr);
		}
		
		LastTimeStamp = 0;		
	}
	
	if (FireNumber!=Incubator::FireNumberNone && FireNumber<=0 && 0==BornTimeout)
	{
		CommonDeclarations::DeleteObjectRequest(this);
	}
	if (Period==0 && 0==BornTimeout)
	{
		Period=-1;
		CommonDeclarations::DeleteObjectRequest(this);
	}
}

bool Incubator::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened
		
	const char *str, *cfg;
	int res;
	
	str = xml->Value();
	assert(strcmp(str,"obj")==0 || strcmp(str,"node")==0 || strcmp(str,"incubator")==0);

	cfg = xml->Attribute("cfg");

	if (cfg)
	{
		return XMLConfigurable::Parse(cfg);
	}

	AAObject::Parse(xml);	

	if (strcmp(str,"incubator")==0) 
	{	
		res = xml->QueryIntAttribute("period", &Period);
		assert(TIXML_SUCCESS==res);

		int vx=0,vy=0,vz=1;
		res = xml->QueryIntAttribute("dir_x", &vx);
		//assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("dir_y", &vy);
		//assert(TIXML_SUCCESS==res);
		res = xml->QueryIntAttribute("dir_z", &vz);
		//assert(TIXML_SUCCESS==res);

		Direction.x = vx;
		Direction.y = vy;
		Direction.z = vz;

        res = xml->QueryIntAttribute("rndpos_x", &RandX);
        //assert(TIXML_SUCCESS==res);
        res = xml->QueryIntAttribute("rndpos_y", &RandY);
        //assert(TIXML_SUCCESS==res);
        res = xml->QueryIntAttribute("rndpos_z", &RandZ);
        //assert(TIXML_SUCCESS==res);

		/*
		str = xml->Attribute("cfg");
		if (str)
		{
			return XMLConfigurable::Parse(str);
		}*/

		res = xml->QueryIntAttribute("born_tout", &BornTimeout);		

		FireNumber = Incubator::FireNumberNone;
		res = xml->QueryIntAttribute("fire_number", &FireNumber);
		
		//Description.reset(xml->Clone());

		assert(Descriptions.IsEmpty());
		TiXmlElement *xml_element = NULL, *actual_xml=NULL;
		xml_element = xml->FirstChildElement("obj");			
		while (xml_element)
		{	
			str = xml_element->Attribute("cfg");

			SDescription *descr = new SDescription;

			res = xml_element->QueryIntAttribute("x", &descr->x);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("y", &descr->y);
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("z", &descr->z);
			assert(TIXML_SUCCESS==res);

			descr->dir_x = Direction.x;
			descr->dir_y = Direction.y;
			descr->dir_z = Direction.z;

			res = xml_element->QueryIntAttribute("dir_x", &descr->dir_x);
			//assert(TIXML_SUCCESS==res);			
			res = xml_element->QueryIntAttribute("dir_y", &descr->dir_y);
			//assert(TIXML_SUCCESS==res);
			res = xml_element->QueryIntAttribute("dir_z", &descr->dir_z);
			//assert(TIXML_SUCCESS==res);

			descr->period = 0;
			res = xml_element->QueryIntAttribute("period", &descr->period);

			descr->FireNumber = Incubator::FireNumberNone;
			res = xml_element->QueryIntAttribute("fire_number", &descr->FireNumber);

			actual_xml=NULL;
			if (str)
			{
				actual_xml = ObjectsLibrary::GetParsed(str);
			} else
				actual_xml = xml_element;
			
			assert(actual_xml);
			
			if (actual_xml)
			{
				descr->Content = actual_xml->Clone()->ToElement();
				Descriptions.PushBack(descr);
			}

			xml_element = xml_element->NextSiblingElement("obj");
		}

		
	}
	
	return true;
}

Ogre::Vector3 Incubator::GetPosition() const
{
	return Position;
}

void Incubator::CreateObject(SDescription *descr)
{
	if (FireNumber!=Incubator::FireNumberNone)
	{
		++descr->FireNumber;
		if (descr->FireNumber<=0)
			return;
	}
	
	SceneObject *obj = new SceneObject;
	
	TiXmlElement *xml_element = descr->Content;

	int xrand=0, yrand=0, zrand=0;
	if (RandX>0)
		xrand = RandX*rand()/(RAND_MAX+1.0);
	if (RandY>0)
		yrand = RandY*rand()/(RAND_MAX+1.0);
	if (RandZ>0)
		zrand = RandZ*rand()/(RAND_MAX+1.0);

	obj->SetInitialPosition(Position+Ogre::Vector3(descr->x+xrand,descr->y+yrand,descr->z+zrand));
	obj->SetInitialDirection(Ogre::Vector3(descr->dir_x,descr->dir_y,descr->dir_z));

	obj->Parse(xml_element);		
	
	CommonDeclarations::AddObjectRequest(obj);
}



#pragma once
#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Descriptable.h"
#include "ObjectsLibrary.h"
#include "Utilities.h"

//Descriptable::DescriptionsContainer *Descriptable::Descriptions = NULL;

Descriptable::Descriptable() :
//DescriptableContent(NULL),
IconName(NULL),
Header(NULL),
Text(NULL)
{
	//Descriptions = new Descriptable::DescriptionsContainer;
}

Descriptable::~Descriptable()
{
	//if (DescriptableContent)
	//	delete DescriptableContent;
	if (IconName)
		delete [] IconName;
	if (Header)
		delete [] Header;
	if (Text)
		delete [] Text;
}

//TiXmlElement* Descriptable::GetDescription()
//{
//	return DescriptableContent;
//}

Descriptable::DescriptionsContainer *Descriptable::GetDescriptions()
{
	static Descriptable::DescriptionsContainer *Descriptions = new Descriptable::DescriptionsContainer;
	return Descriptions;
}

bool Descriptable::ParseDescription(TiXmlElement* xml)
{
	/*
	<description id="1">
		<icon content="name.png"/>
		<header content="header"/>
		<text content="text"/>
	</description>
	*/
	if (NULL==xml)
		return false;
	TiXmlElement *xml_element = 0;
	const char *str;	
		
	xml_element = xml->FirstChildElement("icon");
	if (xml_element)
	{
		str = xml_element->Attribute("content");
		if (str)
			IconName = AAUtilities::StringCopy(str);
	}
	
	xml_element = xml->FirstChildElement("header");
	if (xml_element)
	{
		str = xml_element->Attribute("content");
		if (str)
			Header = AAUtilities::StringCopy(str);
	}
	
	xml_element = xml->FirstChildElement("text");
	if (xml_element)
	{
		str = xml_element->Attribute("content");
		if (str)
			Text = AAUtilities::StringCopy(str);
	}
	return true;
}

bool Descriptable::Parse(TiXmlElement* xml)
{
	if (NULL==xml)
		return false;
	int res, val;
	TiXmlElement *xml_element = 0;
	
	xml_element = xml->FirstChildElement("description");	
	while (xml_element)
	{
		res = xml_element->QueryIntAttribute("id", &val);
		assert(TIXML_SUCCESS==res);
		if (TIXML_SUCCESS==res)
		{
			Descriptable *descr = new Descriptable();
			descr->ParseDescription(xml_element);
			GetDescriptions()->insert(std::make_pair(val, descr));
		}
		
		xml_element = xml_element->NextSiblingElement("description");
	}	
	
	return true;
}

char *Descriptable::GetIconName() const
{
	return IconName;
}

char *Descriptable::GetHeader() const
{
	return Header;
}

char *Descriptable::GetText() const
{
	return Text;
}

void Descriptable::Init()
{
	//XMLConfigurable::Parse("descriptions.xml");
	Parse(ObjectsLibrary::GetInstance()->GetParsed("descriptions.xml"));
}

void Descriptable::Reset()
{
	DescriptionsContainer *Descriptions = GetDescriptions();
	DescriptionsContainer::iterator iPos = Descriptions->begin(), iEnd = Descriptions->end();
	for (;iPos!=iEnd;++iPos)
	{
		Descriptable* descr = (Descriptable*)iPos->second;
		delete descr;
	}
	Descriptions->clear();
}

void Descriptable::Close()
{		
	delete GetDescriptions();
}

IDescriptable* Descriptable::GetDescription(int id)
{
	DescriptionsContainer *Descriptions = GetDescriptions();
	DescriptionsContainer::iterator iRes = Descriptions->find(id);
	if (Descriptions->end()!=iRes)
	{
		return iRes->second;
	}
	return NULL;	
}


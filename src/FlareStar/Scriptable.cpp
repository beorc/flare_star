#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\aaobject.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "Scriptable.h"

Scriptable::Scriptable():
ID(0),
ScriptHandlersNumber(0)
{
	memset(ScriptValues,0,sizeof(ScriptValues));
	memset(ScriptHandlers,0,sizeof(ScriptHandlers));
}

Scriptable::~Scriptable(void)
{
	ScriptHandlersNumber = sizeof(ScriptHandlers)/sizeof(char*);
	for (size_t i=0;i<ScriptHandlersNumber;++i)
		delete [] ScriptHandlers[i];
}

char *Scriptable::GetScriptHandler(size_t index)
{
	if (index<ScriptHandlersNumber)
		return ScriptHandlers[index];
	return NULL;		
}

void Scriptable::SetID(int id)
{
    ID=id;
}

int Scriptable::GetID()
{
    return ID;
}

bool Scriptable::Parse(TiXmlElement* xml)
{
	    
    if (xml == 0)
        return false; // file could not be opened
		
	xml = XMLConfigurable::CheckLID(xml);
			
    xml->QueryIntAttribute("id",&ID);
	
	TiXmlElement *xml_element = 0;
	const char *handler, *str;	
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
	
    return true;
}

void Scriptable::SetScriptValue(size_t index, char val)
{
	ScriptValues[index] = val;
}

char Scriptable::GetScriptValue(size_t index)
{
	return ScriptValues[index];
}

void Scriptable::SaveTo(TiXmlElement * root) const
{
	 root->SetAttribute("id", ID);
}

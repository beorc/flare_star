#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\stringresources.h"

//StringResources *StringResources::Instance = NULL;

StringResources::StringResources(void):
Strings(new StringsContainer)
{
	//Instance = new StringResources;
}

StringResources::~StringResources(void)
{    
	Reset();
}

char *StringResources::GetString(int num)
{
    return NULL;
}

bool StringResources::Parse(TiXmlElement* xml)
{
    assert(Strings->empty());
    TiXmlElement *xml_element=NULL;
    const char *str;	
    str = xml->Attribute("cfg");
    if (str)
    {
        return XMLConfigurable::Parse(str);
    }

    if (xml == 0)
        return false; // file could not be opened

    //str = xml->Value();

    xml_element = xml;

    int val, res;
    str = xml->Value();

    assert(strcmp("strings",str)==0);

    xml_element = xml->FirstChildElement("string");
    assert(xml_element);

    while (xml_element)
    {
        res = xml_element->QueryIntAttribute("id", &val);
        str = xml_element->Attribute("content");

        Strings->insert(std::make_pair(val,AAUtilities::StringCopy(str)));
        
        xml_element =xml_element->NextSiblingElement("string");
    }	

    return true;
}

void StringResources::Init()
{    
    XMLConfigurable::Parse("english.xml");
}

void StringResources::Reset()
{
	if (Strings)
	{
		StringsContainer::iterator iPos = Strings->begin(), iEnd = Strings->end();
		for (;iPos!=iEnd;++iPos)
		{
			delete [] iPos->second;
		}
		Strings->clear();
	}
}

void StringResources::Close()
{
	delete Strings;
	Strings = NULL;
	delete GetInstance();
}

StringResources *StringResources::GetInstance()
{
	static StringResources *Instance = new StringResources;
    return Instance;
}

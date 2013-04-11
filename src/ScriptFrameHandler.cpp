#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "ScriptFrameHandler.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"

ScriptFrameHandler::ScriptFrameHandler(void)
{
    memset(ScriptValues,0,sizeof(ScriptValues));
    memset(ScriptHandlers,0,sizeof(ScriptHandlers));
}

ScriptFrameHandler::~ScriptFrameHandler(void)
{
	Close();
}

void ScriptFrameHandler::Step(unsigned timeMs)
{    
    if (ScriptHandlers[0])
        ScriptManager::GetInstance()->Call(ScriptHandlers[0], "i", false, timeMs);
}

bool ScriptFrameHandler::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    const char *str;	

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
            if (strcmp(str,"timer")==0) 
            {
                ScriptHandlers[0]=AAUtilities::StringCopy(handler);				
            } else
                if (strcmp(str,"over")==0) 
                {
                    ScriptHandlers[1]=AAUtilities::StringCopy(handler);				
                }
            xml_element = xml_element->NextSiblingElement("handler");
        }
		return true;
    }
            
    return false;
}

void ScriptFrameHandler::Close()
{
	size_t count = sizeof(ScriptHandlers)/sizeof(char*);
	for (size_t i=0;i<count;++i)
	{
		if (ScriptHandlers[i])
		{
			delete [] ScriptHandlers[i];
			ScriptHandlers[i] = NULL;
		}		
	}
}

bool ScriptFrameHandler::KeyReleased( const OIS::KeyEvent &arg )
{
	return false;
}

bool ScriptFrameHandler::MouseMoved( const OIS::MouseEvent &arg )
{
	return false;
}

bool ScriptFrameHandler::MouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	return false;
}


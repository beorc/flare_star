#include "StdAfx.h"
//#include <stdio.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Configuration.h"
#include "Utilities.h"

#define MUSIC_VOLUME 1
#define EFFECTS_VOLUME 2
#define CONFFILE "configuration.xml"

//Configuration *Configuration::Instance = NULL;

Configuration::Configuration(void)
{	
}

Configuration::~Configuration(void)
{
}

Configuration *Configuration::GetInstance()
{
	static Configuration *Instance = new Configuration;
	return Instance;
}

bool Configuration::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	//const char *str;	
    int res, iVal;
	
    TiXmlElement *xml_element = 0;
    xml_element = xml->FirstChildElement("Sound");
    if (xml_element) 
    {				
        res = xml_element->QueryIntAttribute("MusicVolume", &iVal);        
        assert(TIXML_SUCCESS==res);
        //Sound.insert(std::make_pair(MUSIC_VOLUME,iVal));
		Sound[MUSIC_VOLUME]=iVal;

        res = xml_element->QueryIntAttribute("EffectsVolume", &iVal);        
        assert(TIXML_SUCCESS==res);
        //Sound.insert(std::make_pair(EFFECTS_VOLUME,iVal));
		Sound[EFFECTS_VOLUME]=iVal;
    }
	

	return true;
}

bool Configuration::Load()
{
    return ParseDirect(CONFFILE);
}

bool Configuration::Save()
{
    TiXmlDocument doc;  
    //TiXmlElement* msg;
    //TiXmlComment * comment;
    std::string s;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
    doc.LinkEndChild( decl ); 
    
	SaveTo(doc.ToElement());
	
	doc.SaveFile(CONFFILE);        
	return true;
}

float Configuration::GetMusicVolume() const
{
    IntConfigBlock::const_iterator iRes = Sound.find(MUSIC_VOLUME);
    if (iRes==Sound.end()) 
        return 50;

    return iRes->second;
}

float Configuration::GetEffectsVolume() const
{
    IntConfigBlock::const_iterator iRes = Sound.find(EFFECTS_VOLUME);
    if (iRes==Sound.end()) 
        return 50;

    return iRes->second;
}

void Configuration::SetMusicVolume(float val)
{
	//IntConfigBlock::const_iterator iRes = Sound.find(MUSIC_VOLUME);
	//if (iRes!=Sound.end())
	//{
	//	Sound[iRes->first]=val;
	//}
	Sound[MUSIC_VOLUME] = val;	
}

void Configuration::SetEffectsVolume(float val)
{
	/*IntConfigBlock::const_iterator iRes = Sound.find(EFFECTS_VOLUME);
	if (iRes!=Sound.end())
	{
		Sound[iRes->first]=val;
	}*/
	//Sound.insert(std::make_pair(AAUtilities::StringCopy(EFFECTS_VOLUME), val));	
	Sound[EFFECTS_VOLUME] = val;	
}

void Configuration::SaveTo(TiXmlElement * root) const
{
	TiXmlElement * conf = new TiXmlElement("configuration");  
	root->LinkEndChild( conf );  

	
	// block: sound
	{
		TiXmlElement * msgs = new TiXmlElement( "Sound" );  
		conf->LinkEndChild( msgs );  

		IntConfigBlock::const_iterator iRes = Sound.find(MUSIC_VOLUME);
		msgs->SetAttribute("MusicVolume", iRes->second);
		
		iRes = Sound.find(EFFECTS_VOLUME);
		msgs->SetAttribute("EffectsVolume", iRes->second);   
	}
}

void Configuration::Close()
{
	delete GetInstance();
}


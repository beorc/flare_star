#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\profile.h"
#include "CommonDeclarations.h"
#include "Player.h"
#include "Configuration.h"

AAProfile::AAProfile(void) :
CurrentLevel(0),
PlayersProfile(NULL),
MouseSens(1.f),
CameraRotationDamping(0.89f)
{    
    strcpy(ProfileName,"default.xml");
}

AAProfile::~AAProfile(void)
{
    Reset();
}

bool AAProfile::Parse(TiXmlElement* xml)
{
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

    assert(strcmp("profile",str)==0);

    res = xml->QueryIntAttribute("level",&val);
    assert(TIXML_SUCCESS==res);
    CurrentLevel = (unsigned char)val;
	
	xml_element = xml->FirstChildElement("controls");
	assert(xml_element);

	if (xml_element)
	{
		res = xml_element->QueryFloatAttribute("cam_rot_damping",&CameraRotationDamping);
		assert(TIXML_SUCCESS==res);

        res = xml_element->QueryFloatAttribute("mouse_sens",&MouseSens);
        assert(TIXML_SUCCESS==res);        
	}    
		
	xml_element = xml->FirstChildElement("configuration");
	assert(xml_element);

	if (xml_element)
	{
		Configuration::GetInstance()->Parse(xml_element);
	}	

    xml_element = xml->FirstChildElement("player");
    assert(xml_element);

    if (xml_element)
    {
        PlayersProfile = xml_element->Clone();                       		
    }

    return true;
}

void AAProfile::Load()
{
	char profile_path[300];
	sprintf(profile_path,"profiles/%s",ProfileName);
	ParseDirect(profile_path);
}

void AAProfile::Save()
{
	char profile_path[300];
	sprintf(profile_path,"profiles/%s",ProfileName);
    TiXmlDocument doc;  
    //TiXmlElement* msg;
    //TiXmlComment * comment;
    std::string s;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
    doc.LinkEndChild( decl ); 

    //<profile  name="default" level_path="media/level1/">
    TiXmlElement * root = new TiXmlElement("profile");  
    doc.LinkEndChild( root );

    root->SetAttribute("name", ProfileName);
    root->SetAttribute("level", CurrentLevel);

    //IAAObject *object;
    //ObjectsPool::iterator iPos = Objects.begin(), iEnd = Objects.end();
    /*ObjectsPool::ListNode *pos = Objects.GetBegin();
    for (;pos!=NULL;pos=pos->Next)
    {
    	object = pos->Value;
    	object->SaveTo(root);
    }*/

	TiXmlElement * elem = new TiXmlElement("controls");  
	elem->SetAttribute("cam_rot_damping", CameraRotationDamping);
	elem->SetAttribute("mouse_sens", MouseSens);
	root->LinkEndChild( elem );
	
	Configuration::GetInstance()->SaveTo(root);

    CommonDeclarations::GetPlayer()->SaveTo(root);

    doc.SaveFile(profile_path);
}

void AAProfile::SaveConfiguration()
{
	char profile_path[300];
	sprintf(profile_path,"profiles/%s",ProfileName);
	TiXmlDocument doc;  
	//TiXmlElement* msg;
	//TiXmlComment * comment;
	std::string s;
	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl ); 

	//<profile  name="default" level_path="media/level1/">
	TiXmlElement * root = new TiXmlElement("profile");  
	doc.LinkEndChild( root );

	root->SetAttribute("name", ProfileName);
	root->SetAttribute("level", CurrentLevel);

	TiXmlElement * elem = new TiXmlElement("controls");  
	elem->SetDoubleAttribute("cam_rot_damping", CameraRotationDamping);
	elem->SetDoubleAttribute("mouse_sens", MouseSens);
	root->LinkEndChild( elem );

	//IAAObject *object;
	//ObjectsPool::iterator iPos = Objects.begin(), iEnd = Objects.end();
	/*ObjectsPool::ListNode *pos = Objects.GetBegin();
	for (;pos!=NULL;pos=pos->Next)
	{
	object = pos->Value;
	object->SaveTo(root);
	}*/

	Configuration::GetInstance()->SaveTo(root);

	root->LinkEndChild(PlayersProfile->Clone());

	doc.SaveFile(profile_path);
}

void AAProfile::Reset()
{
	if (PlayersProfile)
		delete PlayersProfile;
	PlayersProfile = NULL;
	CurrentLevel = 0;	
}



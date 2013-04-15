#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "MeshSubEffect.h"
#include "CommonDeclarations.h"
#include "Debugging.h"
#include "ObjectsLibrary.h"
#include "IMasterEffect.h"

MeshSubEffect::MeshSubEffect(void) :
BaseSubEffect(),
Size(10),
Modelname(NULL),
OgreEntity(NULL),
OgreRenderQueue(Ogre::RENDER_QUEUE_MAIN)
{
}

MeshSubEffect::~MeshSubEffect(void)
{
    if (OgreEntity)
    {
        Ogre::SceneManager *smgr = CommonDeclarations::GetSceneManager();
        smgr->destroyMovableObject(OgreEntity);		
    }
}

bool MeshSubEffect::Parse(TiXmlElement* xml)
{    
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    const char *str;	
    /*str = xml->Attribute("cfg");
    if (str)
    {
        return XMLConfigurable::Parse(str);
    }*/
    xml = ObjectsLibrary::CheckLID(xml);

    BaseSubEffect::Parse(xml);

    TiXmlElement *xml_element = 0;

    xml_element = xml->FirstChildElement("model");
    if (xml_element)
    {	
        str = xml_element->Attribute("modelname");	
        if (str)
        {
            if (Modelname)
                delete [] Modelname;
			Modelname = AAUtilities::StringCopy(str);
        }
		int res;
		res = xml_element->QueryIntAttribute("render_queue", &OgreRenderQueue);

    }   
    return true;
}

bool MeshSubEffect::Start()
{
	/*char log[100];
	sprintf(log,"start\n");
	Debugging::Log("effects",log);*/
    BaseSubEffect::Start();    
    if (OgreEntity)
        OgreEntity->setVisible(true);
    return false;
}

bool MeshSubEffect::Stop()
{
	/*char log[100];
	sprintf(log,"stop\n");
	Debugging::Log("effects",log);*/
    BaseSubEffect::Stop();
    if (OgreEntity)
        OgreEntity->setVisible(false);
    return false;
}

bool MeshSubEffect::InitEntity()
{
    if (!OgreEntity && Modelname)
    {
        Ogre::SceneManager *SceneMgr = CommonDeclarations::GetSceneManager();

        char *buffer = CommonDeclarations::GenGUID();		
        //char *buffer = new char[50];
        //itoa((int)this,buffer,10);		
        //strcat(buffer,"_entity");
        
		OgreEntity = SceneMgr->createEntity(buffer, Modelname);
		delete [] buffer;

		Ogre::SceneNode *node = Master->GetEffectNode();
		if (RelativePos.squaredLength()>0 || RelOrientation!=Ogre::Quaternion::IDENTITY)
		{
			buffer = CommonDeclarations::GenGUID();
			node = node->createChildSceneNode(buffer, RelativePos, RelOrientation);
			delete [] buffer;
		}
		assert(node);
		node->attachObject(OgreEntity);
		OgreEntity->setRenderQueueGroup(OgreRenderQueue);
		OgreEntity->setVisible(false);

        
        return true;
    }

    return false;
}

bool MeshSubEffect::Init()
{
    InitEntity();
    return true;
}


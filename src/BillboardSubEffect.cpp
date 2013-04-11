#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "CommonDeclarations.h"
#include "BillboardSubEffect.h"
#include "ObjectsLibrary.h"
#include "IMasterEffect.h"
#include "IScenable.h"

BillboardSubEffect::BillboardSubEffect(void) :
BaseSubEffect(),
MaterialName(NULL),
PointRendering(false),
BillborardSetReference(NULL),
OgreBillboard(NULL),
Size(100)
{	
}

BillboardSubEffect::~BillboardSubEffect(void)
{
	if (BillborardSetReference && OgreBillboard)
	{
		BillborardSetReference->removeBillboard(OgreBillboard);		
	}	
}

BillboardSubEffect::BillboardSetsContainer *BillboardSubEffect::GetBillboardSets()
{
	static BillboardSubEffect::BillboardSetsContainer *BillboardSets = new BillboardSubEffect::BillboardSetsContainer(50);
	return BillboardSets;
}

bool BillboardSubEffect::Parse(TiXmlElement* xml)
{    
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    const char *str;
	int res;

    xml = ObjectsLibrary::CheckLID(xml);

    BaseSubEffect::Parse(xml);

    TiXmlElement *xml_element = 0;

    xml_element = xml->FirstChildElement("billboard");
    if (xml_element)
    {	        
        res = xml_element->QueryIntAttribute("size", &Size);
        assert(TIXML_SUCCESS==res);		
		
		str = xml_element->Attribute("point_render");
		if (str && strcmp(str,"on")==0)
			PointRendering = true;
    }

    xml_element = xml->FirstChildElement("material");
    if (xml_element)
    {	
        str = xml_element->Attribute("name");	
        if (str)
        {
            if (MaterialName)
                delete [] MaterialName;
            MaterialName = AAUtilities::StringCopy(str);
        }		
    }
	
    return true;
}

bool BillboardSubEffect::Init()
{
	assert(MaterialName);
	if (MaterialName)
	{
		BillboardSetsContainer *BillboardSets = GetBillboardSets();
		Ogre::BillboardSet *hashres=NULL;
		bool bres = BillboardSets->Find(MaterialName, &hashres);

		if (!bres)
		{
			char *buffer = CommonDeclarations::GenGUID();

			Ogre::SceneManager *scene_manager = CommonDeclarations::GetSceneManager();
			Ogre::BillboardSet *billboards = scene_manager->createBillboardSet(buffer,1);
			billboards->setAutoextend(true);
			scene_manager->getRootSceneNode()->attachObject(billboards);
            
			BillboardSets->Insert(MaterialName,billboards);
			
			delete [] buffer;

			billboards->setMaterialName(MaterialName);
			
			if (PointRendering)
			{
				billboards->setBillboardType(Ogre::BBT_POINT);
				billboards->setBillboardOrigin(Ogre::BBO_CENTER);
				billboards->setPointRenderingEnabled(true);
			}

			BillborardSetReference = billboards;
		} else
			BillborardSetReference = hashres;

		OgreBillboard = BillborardSetReference->createBillboard(Ogre::Vector3::ZERO);
		if (!PointRendering)
		{
			OgreBillboard->setDimensions(Size, Size);
		}
	}    
	
	return true;
}

void BillboardSubEffect::Step(unsigned timeMs)
{
	//if (Parent)
	OgreBillboard->setPosition(Master->GetParent()->GetPosition());
}

void BillboardSubEffect::Reset()
{	
	GetBillboardSets()->Clear();
}

void BillboardSubEffect::Close()
{
	delete GetBillboardSets();
}


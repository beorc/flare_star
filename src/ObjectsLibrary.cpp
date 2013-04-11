#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "ObjectsLibrary.h"

#include "CommonDeclarations.h"
#include "Container.h"
//#include "MeshEffect.h"
//#include "BillboardEffect.h"
#include "GenericAmmo.h"
#include "EulerPhysicsAmmo.h"
#include "BillboardAmmo.h"
#include "GenericWeapon.h"
#include "Bonus.h"

#include "MasterEffect.h"
#include "IAmmo.h"

//ObjectsLibrary *ObjectsLibrary::Instance = NULL;

ObjectsLibrary::ObjectsLibrary(void)
{	
	//Instance = new ObjectsLibrary;
}

ObjectsLibrary::~ObjectsLibrary(void)
{
	Reset();
}

 ObjectsLibrary *ObjectsLibrary::GetInstance()
 {
	static ObjectsLibrary *Instance = new ObjectsLibrary;
	return Instance;
 }
 
 TiXmlElement *ObjectsLibrary::GetParsed(const char *filename)
{   
	TiXmlElement *root=NULL;
	
	/*TiXmlDocument *xml = new TiXmlDocument(filename);	
	    
	bool loadOkay = xml->LoadFile();
	assert(loadOkay);
	    
	if (!loadOkay)
	{
		Ogre::StringUtil::StrStreamType errorMessage;
		errorMessage << "There was an error with the xml file: " << filename;

		OGRE_EXCEPT
		(
			Ogre::Exception::ERR_INVALID_STATE,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
		);
		return false;
	}*/

	//Open the file
	Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	assert(!stream.isNull());
	if (stream.isNull())
	{		
		Ogre::StringUtil::StrStreamType errorMessage;
		errorMessage << "Could not open xml file: " << filename;

		OGRE_EXCEPT
			(
			Ogre::Exception::ERR_FILE_NOT_FOUND,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
			);
	}

	//Get the file contents
	//String data = stream->getAsString();

	// Read the entire buffer	
	size_t Size = stream->size();

	char* pBuf = new char[Size+1];
	// Ensure read from begin of stream
	stream->seek(0);
	stream->read(pBuf, Size);
	pBuf[Size] = '\0';

	//Parse the XML document

	TiXmlDocument *xml = new TiXmlDocument();	
	xml->Parse(pBuf);

	delete [] pBuf;
	stream.setNull();
	assert(!xml->Error());
	if (xml->Error())
	{
		Ogre::StringUtil::StrStreamType errorMessage;
		errorMessage << "There was an error with the xml file: " << filename;

		OGRE_EXCEPT
			(
			Ogre::Exception::ERR_INVALID_STATE,
			errorMessage.str(), 
			"XMLConfigurable::Parse"
			);
		return false;
	}

	root = xml->FirstChildElement();	
	assert( root );
	root=root->Clone()->ToElement();
		
	delete xml;

	return root;
}

bool ObjectsLibrary::Parse(TiXmlElement* xml)
 {
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	int res, val;
	const char *str;		
	str = xml->Attribute("cfg");
	if (str)
	{
		xml = GetParsed(str);
	}
	
	str = xml->Value();

	TiXmlElement *xml_element = 0, *root;
	xml_element = xml->FirstChildElement("object");
	
	while (xml_element)
	{
		res = xml_element->QueryIntAttribute("lid",&val);
		assert(TIXML_SUCCESS==res);
		str = xml_element->Attribute("filename");
		root = GetParsed(str);
		assert(root);
		Library.insert( std::make_pair(val,root) );
		xml_element = xml_element->NextSiblingElement("object");
	}
	
	return true;
 }
 
 TiXmlElement *ObjectsLibrary::Get(int lid)
 {
	LibraryContainer::iterator iRes = Library.find(lid);

	TiXmlElement *root=NULL;
	if (iRes!=Library.end())
	{
		root = iRes->second->ToElement();
	}
	assert(root);
	return root;
 }
 
 IAAObject *ObjectsLibrary::Load(int lid)
 {
	assert(lid);
	TiXmlElement *xml = Get(lid);
	assert(xml);
	const char *str;

	str = xml->Value();
	if (strcmp(str,"trigger")==0) 
	{
				str = xml->Attribute("class");
				if (strcmp(str,"container")==0)
				{
					Container *cont = new Container;
					//MALLOC(cont,Container,);
					cont->Parse(xml);
					cont->SetLID(lid);
					return cont;
				}
	}
	
	/*if (strcmp(str,"effect")==0) 
	{
		IEffect *effect=NULL;
		str = xml->Attribute("class");
        assert(str);
        if (strcmp(str,"MeshEffect")==0)
        {
            effect = new MeshEffect();
        } else
            if (strcmp(str,"BillboardEffect")==0)
            {
                effect = new BillboardEffect();
            }
		effect->Parse(xml);
		return effect;
	}*/
	/*if (strcmp(str,"ammo")==0)
	{
		GenericAmmo *ammo=NULL;
		str = xml->Attribute("class");
		
		if (strcmp(str,"GenericAmmo")==0)
		{
			//ammo =  new (GenericAmmo::GenericAmmoAllocator) GenericAmmo(&GenericAmmo::GenericAmmoAllocator);
			PMALLOC(ammo, GenericAmmo, GenericAmmo::GenericAmmoAllocator);		
		} else
			if (strcmp(str,"EulerPhysicsAmmo")==0)
			{
				//ammo = new (EulerPhysicsAmmo::EulerPhysicsAmmoAllocator) EulerPhysicsAmmo(&EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
				PMALLOC(ammo, EulerPhysicsAmmo, EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
			} else
			if (strcmp(str,"BillboardAmmo")==0)
			{
				//ammo = new (EulerPhysicsAmmo::EulerPhysicsAmmoAllocator) EulerPhysicsAmmo(&EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
				PMALLOC(ammo, BillboardAmmo, EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
			}
		ammo->Parse(xml);
		return ammo;
	}
	*/
	
	return NULL;
 }
 
 void ObjectsLibrary::Reset()
 {
	 LibraryContainer::iterator iPos = Library.begin(), iEnd = Library.end();
	 for (;iPos!=iEnd;++iPos)
	 {
		 delete iPos->second;
	 }
	 Library.clear();	 
 }
 
 void ObjectsLibrary::Close()
 {
	 delete GetInstance();
 }

 IMasterEffect *ObjectsLibrary::LoadEffect(int lid)
 {
	assert(lid);
	TiXmlElement *xml = Get(lid);
	assert(xml);
	const char *str;

	str = xml->Value();

	if (strcmp(str,"effect")==0) 
	{
		MasterEffect *effect = new MasterEffect;
		
		effect->SetLID(lid);
		effect->Parse(xml);
		return effect;
	}
	return NULL;
 }
 
 IAmmo *ObjectsLibrary::LoadAmmo(int lid)
 {
	 assert(lid);
	 TiXmlElement *xml = Get(lid);
	 assert(xml);
	 const char *str;

	 str = xml->Value();
	if (strcmp(str,"ammo")==0)
	{
		IAmmo *ammo=NULL;
		str = xml->Attribute("class");
		
		if (strcmp(str,"GenericAmmo")==0)
		{
			//ammo =  new (GenericAmmo::GenericAmmoAllocator) GenericAmmo(&GenericAmmo::GenericAmmoAllocator);
			//PMALLOC(ammo, GenericAmmo, GenericAmmo::GenericAmmoAllocator);		
			ammo = new GenericAmmo;
		} else
			if (strcmp(str,"EulerPhysicsAmmo")==0)
			{
				//ammo = new (EulerPhysicsAmmo::EulerPhysicsAmmoAllocator) EulerPhysicsAmmo(&EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
				//PMALLOC(ammo, EulerPhysicsAmmo, EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
				ammo = new EulerPhysicsAmmo;
			} else
			if (strcmp(str,"BillboardAmmo")==0)
			{
				//ammo = new (EulerPhysicsAmmo::EulerPhysicsAmmoAllocator) EulerPhysicsAmmo(&EulerPhysicsAmmo::EulerPhysicsAmmoAllocator);
				//PMALLOC(ammo, BillboardAmmo, BillboardAmmo::BillboardAmmoAllocator);
				ammo = new BillboardAmmo;
			}
		ammo->SetLID(lid);
		ammo->Parse(xml);
		return ammo;
	}
	return NULL;
 }

 Weapon *ObjectsLibrary::LoadWeapon(int lid)
 {
	 assert(lid);
	 TiXmlElement *xml = Get(lid);
	 assert(xml);
	 const char *str;

	 str = xml->Value();
	 if (strcmp(str,"weapon")==0) 
	 {
		 str = xml->Attribute("class");
		 if (strcmp(str,"GenericWeapon")==0)
		 {
			 GenericWeapon *cont = new GenericWeapon();
			 cont->Parse(xml);
			 cont->SetLID(lid);
			 return cont;
		 }
	 }
	 return NULL;
 }

 Bonus *ObjectsLibrary::LoadBonus(int lid)
 {
	 assert(lid);
	 TiXmlElement *xml = Get(lid);
	 assert(xml);
	 const char *str;

	 str = xml->Value();
	 if (strcmp(str,"bonus")==0) 
	 {
		Bonus *bonus = new Bonus();
		bonus->Parse(xml);
		return bonus;

	 }
	 return NULL;
 }

 TiXmlElement* ObjectsLibrary::CheckLID(TiXmlElement* xml)
 {
     int res;	
     int LID;
     res = xml->QueryIntAttribute("lid", &LID);	
     if (TIXML_SUCCESS==res)
     {
         return ObjectsLibrary::GetInstance()->Get(LID);
     }
     return xml;
 }


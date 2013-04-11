#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\XMLConfigurable.h"
#include "ObjectsLibrary.h"
#include "Debugging.h"

// TODO preload and parse all xml files

//XMLConfigurable::ParsedFilesPool *XMLConfigurable::ParsedFiles = NULL;

XMLConfigurable::XMLConfigurable(void) :
CfgReference(NULL)
//LID(0)
{
	//ParsedFiles = new ParsedFilesPool(100);	
}

XMLConfigurable::~XMLConfigurable(void)
{
	if (CfgReference)
		delete [] CfgReference;
}

XMLConfigurable::ParsedFilesPool *XMLConfigurable::GetParsedFiles()
{
	static ParsedFilesPool *ParsedFiles = new ParsedFilesPool(100);
	return ParsedFiles;
}

bool XMLConfigurable::ParseDirect(const char *filename)
{    
	//ParsedFilesPool::iterator iRes = ParsedFiles->find(filename);
    ParsedFilesPool *ParsedFiles = GetParsedFiles();

	TiXmlNode *res_node=NULL;
    bool bres = ParsedFiles->Find(filename, &res_node);

    TiXmlElement *root=NULL;
	if (!bres)
	{
		TiXmlDocument *xml = new TiXmlDocument(filename);	
	    
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
		}

		root = xml->FirstChildElement();	
		assert( root );
		root=root->Clone()->ToElement();
		ParsedFiles->Insert(filename,root);
		// delete the xml parser after usage
		delete xml;
	} else
	{
		root = res_node->ToElement();
	}
	Parse(root);
    
    return true;
}

bool XMLConfigurable::Parse(const char *filename)
{	
	//ParsedFilesPool::iterator iRes = ParsedFiles->find(filename);
    TiXmlNode *node_res=NULL;
	ParsedFilesPool *ParsedFiles = GetParsedFiles();

    bool bres = ParsedFiles->Find(filename, &node_res);

	if (NULL==CfgReference)
		CfgReference = AAUtilities::StringCopy(filename);
	
	TiXmlElement *root=NULL;
	if (!bres)
	{
		/*char log[100];
		sprintf(log,"parse file %s\n",filename);	
		Debugging::Log("xmlconfigurable",log);*/

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
		root = root->Clone()->ToElement();
		ParsedFiles->Insert(filename,root);
		// delete the xml parser after usage
		delete xml;
	} else
	{
		root = node_res->ToElement();
	}
	Parse(root);

	return true;
}

bool XMLConfigurable::Parse(TiXmlElement* xml)
{
	assert(xml);	
	return false;
}

void XMLConfigurable::Reset()
{
	ParsedFilesPool *ParsedFiles = GetParsedFiles();
    Array<TiXmlNode*> *data = ParsedFiles->GetData();
    for (size_t i=0;i<data->Size;++i)
    {
        if (data->Data[i])
            delete data->Data[i];
    }
    ParsedFiles->Clear();
	/*ParsedFilesPool::iterator iPos=ParsedFiles->begin(), iEnd=ParsedFiles->end();
	AAUtilities::StrKeyType name;
	for (;iPos!=iEnd;++iPos)
	{
		name = iPos->first;
		delete [] name;
		delete iPos->second;
	}
	ParsedFiles->clear();*/	
}

void XMLConfigurable::Close()
{
	Reset();
	delete GetParsedFiles();	
}

TiXmlElement* XMLConfigurable::GetNode(const char *filename)
{
	//ParsedFilesPool::iterator iRes = ParsedFiles->find(filename);
    TiXmlNode *node_res=NULL;
    bool bres = GetParsedFiles()->Find(filename, &node_res);

	TiXmlElement *root=NULL;
	if (bres)
	{
		root = node_res->ToElement();
	}
	return root;
}

TiXmlElement* XMLConfigurable::CheckLID(TiXmlElement* xml)
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

int res, val;
//res = xml->QueryIntAttribute("lid", &val);
//if (TIXML_SUCCESS==res)
//{	
//	//return XMLConfigurable::Parse(str);
//	xml = ObjectsLibrary::GetInstance()->Get(val);
//	assert(xml);
//	if (!xml)
//		return false;
//	//return Parse(xml);
//}

bool XMLConfigurable::ParseLID(TiXmlElement* xml)
{
	int res;	
	int LID;
	res = xml->QueryIntAttribute("lid", &LID);		
	if (TIXML_SUCCESS==res)
	{		
		TiXmlElement *xml_element;
		xml_element = ObjectsLibrary::GetInstance()->Get(LID);
		assert(xml_element);
		if (!xml_element)
			return false;
		return Parse(xml_element);
	}
	return false;
}
//
//int XMLConfigurable::GetLID() const
//{
//	return LID;
//}



#pragma once

#include "Utilities.h"
#include "HashMap.h"

class TiXmlElement;
class TiXmlNode;

class XMLConfigurable
{
public:
	XMLConfigurable(void);
	virtual ~XMLConfigurable(void);

    virtual bool ParseDirect(const char *filename);
	virtual bool Parse(const char *filename);
	virtual bool Parse(TiXmlElement* xml);

	static void Reset();
	static void Close();
	
	TiXmlElement *GetNode(const char *filename);
	
	TiXmlElement* CheckLID(TiXmlElement* xml);
	bool ParseLID(TiXmlElement* xml);
	//int GetLID() const;

	
protected:
	char *CfgReference;
	//int LID;
private:
	
	//typedef std::hash_map< AAUtilities::StrKeyType, TiXmlNode * > ParsedFilesPool; /*,std::hash_compare <AAUtilities::StrKeyType, AAUtilities::greater_str >*/
    typedef AAHashMap<TiXmlNode*> ParsedFilesPool;

	//static ParsedFilesPool *ParsedFiles;
	static ParsedFilesPool *GetParsedFiles();
};

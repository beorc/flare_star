#pragma once
#include "IDescriptable.h"
#include "CommonDeclarations.h"

class Descriptable : public IDescriptable
{
public:
	//typedef int KeyType;
	//typedef std::hash_map< KeyType, IDescriptable*,std::hash_compare <KeyType, CommonDeclarations::greater_int > > DescriptionsContainer;
	typedef std::map< int, IDescriptable* > DescriptionsContainer;

	Descriptable();
	~Descriptable();
	
	//TiXmlElement* GetDescription();
	char *GetIconName() const;
	char *GetHeader() const;
	char *GetText() const;
		
	static void Init();
	static void Close();
	static void Reset();
	static IDescriptable* GetDescription(int id);
protected:
	//TiXmlElement* DescriptableContent;
	char *IconName;
	char *Header;
	char *Text;
	virtual bool ParseDescription(TiXmlElement* xml);
	static bool Parse(TiXmlElement* xml);
	
private:
	//static DescriptionsContainer *Descriptions;
	static DescriptionsContainer *GetDescriptions();
};



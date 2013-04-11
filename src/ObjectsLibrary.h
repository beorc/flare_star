#pragma once
#include "CommonDeclarations.h"

class IMasterEffect;
class IAmmo;
class Weapon;
class Bonus;

class ObjectsLibrary
{
public:    
	//typedef int KeyType;
	//typedef std::hash_map< KeyType, TiXmlNode *,std::hash_compare <KeyType, CommonDeclarations::greater_int > > LibraryContainer;
	typedef std::map< int, TiXmlNode *> LibraryContainer;
    ~ObjectsLibrary(void);

    static ObjectsLibrary *GetInstance();        

    bool Parse(TiXmlElement* xml);
	TiXmlElement *Get(int lid);	
	
	IAAObject *Load(int lid);
	IMasterEffect	*LoadEffect(int lid);
	IAmmo *LoadAmmo(int lid);
	Weapon *LoadWeapon(int lid);
	Bonus *LoadBonus(int lid);
	void Reset();
	static void Close();

	static TiXmlElement *GetParsed(const char *filename);
    static TiXmlElement* CheckLID(TiXmlElement* xml);
protected:
    ObjectsLibrary(void);    

    //static ObjectsLibrary *Instance;	
	
	LibraryContainer Library;
};

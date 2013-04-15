#pragma once
#include "XMLConfigurable.h"
#include <map>

class StringResources : public XMLConfigurable
{
public:    
    typedef std::map<int, char*> StringsContainer;
    ~StringResources(void);

    virtual bool Parse(TiXmlElement* xml);
    char *GetString(int num);

    void Init();
    void Reset();
	void Close();

    static StringResources *GetInstance();
protected:
    StringResources(void);
    //static StringResources *Instance;

    StringsContainer *Strings;    
};

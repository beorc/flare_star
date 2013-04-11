#pragma once
#include "XMLConfigurable.h"

struct SQVM;


class ScriptManager : XMLConfigurable
{
public:    
    ~ScriptManager(void);

    static ScriptManager *GetInstance();        

    bool Parse(TiXmlElement* xml);
    SQVM *GetVM();
    virtual bool Call(char *name, char *format, bool riseerror, ...);
	void Reset();

	static void Close();
	void Init();	
    
protected:
    ScriptManager(void);
    
    int CompileFile(const char *filename);
	void RegisterFunctions();

    //static ScriptManager *Instance;
    SQVM *VM;    
};

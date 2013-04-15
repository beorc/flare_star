#pragma once

#include "XMLConfigurable.h"
#include "IScriptable.h"

class Scriptable : public XMLConfigurable, public IScriptable
{
public:	
	Scriptable();
	virtual ~Scriptable(void);

	virtual int GetID();
	virtual void SetID(int id);
	virtual void SetScriptValue(size_t index, char val);
	virtual char GetScriptValue(size_t index);
    virtual bool Parse(TiXmlElement* xml);
	virtual void SaveTo(TiXmlElement * root) const;
	
	char *GetScriptHandler(size_t index);
protected:
    int ID;
	size_t ScriptHandlersNumber;
	char ScriptValues[10];	
	char *ScriptHandlers[3];	
};

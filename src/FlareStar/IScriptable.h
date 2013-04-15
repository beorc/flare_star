#pragma once

class IScriptable
{
public:    
    virtual int GetID() = 0;
    virtual void SetID(int id) = 0;
    virtual void SetScriptValue(int index, char val) = 0;
    virtual char GetScriptValue(int index) = 0;    
};

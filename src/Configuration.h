#pragma once
#include "xmlconfigurable.h"
#include "ISerializable.h"

class Configuration : public XMLConfigurable, public ISerializable
{
public:	
    typedef std::map<int, int> IntConfigBlock;
	~Configuration(void);

    static Configuration *GetInstance();

	virtual bool Parse(TiXmlElement* xml);
    virtual bool Load();
	virtual bool Save();
	virtual void SaveTo(TiXmlElement * root) const;

    float GetMusicVolume() const;
    float GetEffectsVolume() const;

	void SetMusicVolume(float val);
	void SetEffectsVolume(float val);

	static void Close();

protected:
	Configuration(void);

	//static Configuration *Instance;	

    IntConfigBlock Sound;    
};

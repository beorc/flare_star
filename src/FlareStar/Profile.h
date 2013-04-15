#pragma once

#include "XMLConfigurable.h"

struct AAProfile : public XMLConfigurable
{
public:
    AAProfile(void);
    ~AAProfile(void);
    
    virtual bool Parse(TiXmlElement* xml);

	void Load();
    void Save();
	void Reset();
	void SaveConfiguration();

    char ProfileName[100];
    unsigned char CurrentLevel;
    TiXmlNode* PlayersProfile;
	float CameraRotationDamping, MouseSens;
};

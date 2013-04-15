#pragma once

struct Stall
{
public:   
    Stall(void);    
    virtual ~Stall(void);

	void Load();
    void Init();
    void Close();
	bool Parse(TiXmlElement* xml);

    void SetPosition(Ogre::Rectangle2D *rect, Ogre::Vector3 pos, float size);

    Ogre::MaterialPtr StallingMaterial, ForeStallingMaterial;
    Ogre::Rectangle2D *Stalling, *Forestalling;
	float StallSize, ForestallSize;
	float AspectRatio;
};

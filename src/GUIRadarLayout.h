#pragma once
#include "guilayout.h"
#include "CommonDeclarations.h"
#include "Array.h"

class GUIRadarLayout :
	public GUILayout
{
public:
	enum ECompasTargetType {CTT_NONE,CTT_OBJECT,CTT_POINT};
	GUIRadarLayout(const char *name);
	virtual ~GUIRadarLayout(void);

	virtual void Hide();
	virtual void Show(GUILayout *back_layout=NULL);

	virtual bool Load();
	virtual bool Init();
    bool Update(CommonDeclarations::RadarObjectsArray &);

    void SetCompasTarget(Ogre::Vector3 pos);
	void SetCompasTarget(int id, int wait_timeout);
    void RemoveCompasTarget();
    void DrawCompas(Ogre::Quaternion players_orientation);
	void InitCompasRect(Ogre::Rectangle2D *&rect);
	
	Ogre::Vector3 GetCompasTargetPoint() const;
	int GetCompasTargetID() const;

	ECompasTargetType GetCompasTargetType() const;
protected:
	//Ogre::TexturePtr  RadarTextureDest;
    //Ogre::TexturePtr RadarTextureSrc;
    //Ogre::HardwarePixelBufferSharedPtr  PixelBufferDest, PixelBufferSrc;

    //Ogre::Image::Box AreaDest, AreaSrc;

	Ogre::Rectangle2D *rect, *CompasRect, *CompasObj;

	//Ogre::ManualObject* manual;

	//void AddRow();
	//void AddRow();
	void DrawPos(unsigned num, Ogre::Vector3 dst_pos, Ogre::Quaternion players_orientation);
	//unsigned RowsNumber;

	//Array<Ogre::ManualObject*> Rows;
	Array<Ogre::Rectangle2D*> Rows;
	Array<Ogre::Rectangle2D*> Objects;
	Ogre::Real AspectRatio;
    //bool HaveCompasTarget;
    Ogre::Vector3 CompasTarget;
	Ogre::MaterialPtr CompasMaterial, RadarObjMaterial, RadarRowMaterial;
	int TargetID, WaitCompasTargetTimeout;
	bool Active;
	ECompasTargetType TargetType;
};

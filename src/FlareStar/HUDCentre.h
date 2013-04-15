#pragma once
#include <Ogre.h>
#include "Array.h"

class HUDCentre
{
public:
	struct SRect
	{
		enum ECoordMode {ECM_NONE, ECM_CENTER};
		float Left, Top ,W ,H;		
		ECoordMode CoordMode;

		void ReformRect(int vp_w, int vp_h);
	};
	struct SSlot 
	{
		Ogre::Rectangle2D* Rect;
		int ID;
	};
    ~HUDCentre(void);

    static HUDCentre *GetInstance();    
	
	bool Load();
	bool Init();
	void Close();
	void Reset();
	bool Parse(TiXmlElement* xml);

	void Show();
	void Hide();

	void SetAutoPilotActive(bool on);

	void AddEqupment(int id);
	void AddBonus(int id);

	void RemoveEqupment(int id);
	void RemoveBonus(int id);

	void ShowGuiltyConscienceOverlay(bool show);
    	
protected:	
    HUDCentre(void);

	Array<SSlot> Bonuses;
	Array<SSlot> Equipments;
	Ogre::Rectangle2D* AutoPilotIndicator;
	char AutoPilotMaterialNameActive[100], AutoPilotMaterialNameInactive[100], EquipmentInactive[100], BonusInactive[100];
	int HUDRenderQueue;
	SRect AutoPilotRect, EquipmentStartRect, BonusesStartRect;
	float BorderWidth;
	bool GuiltyConscienceOverlayVisible;
};

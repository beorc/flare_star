#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "HUDCentre.h"
#include "CommonDeclarations.h"
#include "IDescriptable.h"
#include "Descriptable.h"
#include "AACamera.h"
#include "AApplication.h"

HUDCentre::HUDCentre(void) :
AutoPilotIndicator(NULL),
HUDRenderQueue(Ogre::RENDER_QUEUE_6),
BorderWidth(0),
GuiltyConscienceOverlayVisible(false)
{
	memset(AutoPilotMaterialNameActive, 0, sizeof(AutoPilotMaterialNameActive));
	memset(AutoPilotMaterialNameInactive, 0, sizeof(AutoPilotMaterialNameInactive));
	memset(EquipmentInactive, 0, sizeof(EquipmentInactive));
	memset(BonusInactive, 0, sizeof(BonusInactive));	
}

HUDCentre::~HUDCentre(void)
{
	for (size_t i=0;i<Bonuses.Size;++i)
	{	
		if (Bonuses.Data[i].Rect)
		{
			delete Bonuses.Data[i].Rect;
			Bonuses.Data[i].Rect = NULL;
		}		
	}

	for (size_t i=0;i<Equipments.Size;++i)
	{	
		if (Equipments.Data[i].Rect)
		{
			delete Equipments.Data[i].Rect;
			Equipments.Data[i].Rect = NULL;
		}		
	}

	if (AutoPilotIndicator)
	{
		delete AutoPilotIndicator;
		AutoPilotIndicator = NULL;
	}	
}

HUDCentre *HUDCentre::GetInstance()
{
	static HUDCentre *Instance = new HUDCentre;	
    return Instance;
}

bool HUDCentre::Load()
{
	Ogre::Viewport *vp = CommonDeclarations::GetApplication()->GetCurrentViewPort();
	int vp_w = vp->getActualWidth(), vp_h = vp->getActualHeight();

	AutoPilotRect.ReformRect(vp_w, vp_h);
	EquipmentStartRect.ReformRect(vp_w, vp_h);
	BonusesStartRect.ReformRect(vp_w, vp_h);

	float AspectRatio = CommonDeclarations::GetCamera()->GetOgreCamera()->getAspectRatio();
	AutoPilotIndicator = new Ogre::Rectangle2D(TRUE);
	AutoPilotIndicator->setCorners(AutoPilotRect.Left, AutoPilotRect.Top, AutoPilotRect.Left+AutoPilotRect.W/AspectRatio, AutoPilotRect.Top-AutoPilotRect.H);		
	AutoPilotIndicator->setRenderQueueGroup(HUDRenderQueue);

	float left=0;
	for (size_t i =0; i<Equipments.Size; ++i)
	{
		Equipments.Data[i].Rect = new Ogre::Rectangle2D(TRUE);
		left = EquipmentStartRect.Left + i*(EquipmentStartRect.W+BorderWidth);
		Equipments.Data[i].Rect->setCorners(left, EquipmentStartRect.Top, left+EquipmentStartRect.W/AspectRatio, EquipmentStartRect.Top-EquipmentStartRect.H);		
		Equipments.Data[i].Rect->setRenderQueueGroup(HUDRenderQueue);		
	}
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		Bonuses.Data[i].Rect = new Ogre::Rectangle2D(TRUE);
		left = BonusesStartRect.Left + i*(BonusesStartRect.W+BorderWidth);
		Bonuses.Data[i].Rect->setCorners(left, BonusesStartRect.Top, left+BonusesStartRect.W/AspectRatio, BonusesStartRect.Top-BonusesStartRect.H);		
		Bonuses.Data[i].Rect->setRenderQueueGroup(HUDRenderQueue);		
	}

	return true;
}

bool HUDCentre::Init()
{	
	Ogre::SceneNode *root = CommonDeclarations::GetSceneManager()->getRootSceneNode();
	root->attachObject(AutoPilotIndicator);

	for (size_t i =0; i<Equipments.Size; ++i)
	{
		root->attachObject(Equipments.Data[i].Rect);		 
	}
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		root->attachObject(Bonuses.Data[i].Rect);		 
	}
	GuiltyConscienceOverlayVisible = false;
	
    return true;
}

void HUDCentre::Close()
{
	delete GetInstance();
}

void HUDCentre::Reset()
{	
	SetAutoPilotActive(false);

	for (size_t i =0; i<Equipments.Size; ++i)
	{		
		Equipments.Data[i].Rect->setMaterial(EquipmentInactive);
		Equipments.Data[i].ID = 0;
	}
	for (size_t i =0; i<Bonuses.Size; ++i)
	{		
		Bonuses.Data[i].Rect->setMaterial(BonusInactive);
		Bonuses.Data[i].ID = 0;
	}
}

void HUDCentre::SRect::ReformRect(int vp_w, int vp_h)
{
	if (W>2)
	{
		W = W/vp_w;
		W *= 2;
	}

	if (H>2)
	{
		H = H/vp_h;
		H *= 2;
	}

	if (ECM_CENTER == CoordMode)
	{
		Left -= W/2;
		Top += H/2;
	}
}

bool HUDCentre::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	int res=0;
	const char *str = NULL, *mode=NULL;
	TiXmlElement *xml_element = 0;
	
	res = xml->QueryIntAttribute("render_queue", &HUDRenderQueue);
	res = xml->QueryFloatAttribute("border", &BorderWidth);
		
	xml_element = xml->FirstChildElement("auto_pilot");
	if (xml_element) 
	{
		str = xml_element->Attribute("material_active");
		if (str)
			strcpy(AutoPilotMaterialNameActive, str);
		str = xml_element->Attribute("material_inactive");
		if (str)
			strcpy(AutoPilotMaterialNameInactive, str);
				
		res = xml_element->QueryFloatAttribute("x", &AutoPilotRect.Left);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("y", &AutoPilotRect.Top);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("w", &AutoPilotRect.W);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("h", &AutoPilotRect.H);
		assert(TIXML_SUCCESS==res);

		mode = xml_element->Attribute("mode");

		AutoPilotRect.CoordMode = SRect::ECM_NONE;
		if (mode && strcmp(mode,"center")==0)
			AutoPilotRect.CoordMode = SRect::ECM_CENTER;		
	}
	xml_element = xml->FirstChildElement("equipment");
	if (xml_element) 
	{
		int val=0;
		res = xml_element->QueryIntAttribute("number", &val);
		Equipments.Resize(val);

		str = xml_element->Attribute("material_inactive");
		if (str)
			strcpy(EquipmentInactive, str);

		res = xml_element->QueryFloatAttribute("x", &EquipmentStartRect.Left);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("y", &EquipmentStartRect.Top);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("w", &EquipmentStartRect.W);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("h", &EquipmentStartRect.H);
		assert(TIXML_SUCCESS==res);

		mode = xml_element->Attribute("mode");

		EquipmentStartRect.CoordMode = SRect::ECM_NONE;
		if (mode && strcmp(mode,"center")==0)
			EquipmentStartRect.CoordMode = SRect::ECM_CENTER;
	}

	xml_element = xml->FirstChildElement("bonuses");
	if (xml_element) 
	{
		int val=0;
		res = xml_element->QueryIntAttribute("number", &val);
		Bonuses.Resize(val);

		str = xml_element->Attribute("material_inactive");
		if (str)
			strcpy(BonusInactive, str);

		res = xml_element->QueryFloatAttribute("x", &BonusesStartRect.Left);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("y", &BonusesStartRect.Top);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("w", &BonusesStartRect.W);
		assert(TIXML_SUCCESS==res);
		res = xml_element->QueryFloatAttribute("h", &BonusesStartRect.H);
		assert(TIXML_SUCCESS==res);

		mode = xml_element->Attribute("mode");

		BonusesStartRect.CoordMode = SRect::ECM_NONE;
		if (mode && strcmp(mode,"center")==0)
			BonusesStartRect.CoordMode = SRect::ECM_CENTER;
	}

	return true;
}

void HUDCentre::Show()
{	
	if (AutoPilotIndicator)
		AutoPilotIndicator->setVisible(true);
	for (size_t i =0; i<Equipments.Size; ++i)
	{
		if (Equipments.Data[i].Rect)
			Equipments.Data[i].Rect->setVisible(true);
	}
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		if (Bonuses.Data[i].Rect)
			Bonuses.Data[i].Rect->setVisible(true);
	}	
}

void HUDCentre::Hide()
{
	if (AutoPilotIndicator)
		AutoPilotIndicator->setVisible(false);

	for (size_t i =0; i<Equipments.Size; ++i)
	{
		if (Equipments.Data[i].Rect)
			Equipments.Data[i].Rect->setVisible(false);
	}
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		if (Bonuses.Data[i].Rect)
			Bonuses.Data[i].Rect->setVisible(false);
	}
	ShowGuiltyConscienceOverlay(false);
}

void HUDCentre::SetAutoPilotActive(bool on)
{
	if (AutoPilotIndicator)
	{
		if (on)
		{
			AutoPilotIndicator->setMaterial(AutoPilotMaterialNameActive);
		} else
		{
			AutoPilotIndicator->setMaterial(AutoPilotMaterialNameInactive);
		}
	}
		
}

void HUDCentre::AddEqupment(int id)
{
	IDescriptable *DescriptableReference = Descriptable::GetDescription(id);
	if (NULL==DescriptableReference)
		return;
	char *str = DescriptableReference->GetIconName();
	if (NULL==str)
		return;
	for (size_t i =0; i<Equipments.Size; ++i)
	{
		if (Equipments.Data[i].Rect!=NULL && 0==Equipments.Data[i].ID)
		{
			Equipments.Data[i].Rect->setMaterial(str);
			Equipments.Data[i].ID = id;
			break;
		}
	}
}

void HUDCentre::AddBonus(int id)
{
	IDescriptable *DescriptableReference = Descriptable::GetDescription(id);
	if (NULL==DescriptableReference)
		return;
	char *str = DescriptableReference->GetIconName();
	if (NULL==str)
		return;
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		if (Bonuses.Data[i].Rect!=NULL && 0==Bonuses.Data[i].ID)
		{
			Bonuses.Data[i].Rect->setMaterial(str);
			Bonuses.Data[i].ID = id;
			break;
		}
	}
}

void HUDCentre::RemoveEqupment(int id)
{
	for (size_t i =0; i<Equipments.Size; ++i)
	{
		if (Equipments.Data[i].Rect!=NULL && id==Equipments.Data[i].ID)
		{
			Equipments.Data[i].Rect->setMaterial(EquipmentInactive);
			Equipments.Data[i].ID = 0;
			break;
		}
	}
}

void HUDCentre::RemoveBonus(int id)
{
	for (size_t i =0; i<Bonuses.Size; ++i)
	{
		if (Bonuses.Data[i].Rect!=NULL && id==Bonuses.Data[i].ID)
		{
			Bonuses.Data[i].Rect->setMaterial(BonusInactive);
			Bonuses.Data[i].ID = 0;
			break;
		}
	}
}

void HUDCentre::ShowGuiltyConscienceOverlay(bool show)
{   
	if (GuiltyConscienceOverlayVisible==show)
		return;
	Overlay* o = OverlayManager::getSingleton().getByName("GuiltyConscienceOverlay");
	if (!o)
		OGRE_EXCEPT( Exception::ERR_ITEM_NOT_FOUND, "Could not find overlay GuiltyConscienceOverlay",
		"ShowGuiltyConscienceOverlay" );	
	if (show)
		o->show();
	else
		o->hide();

	GuiltyConscienceOverlayVisible = show;
}




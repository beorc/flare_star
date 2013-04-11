#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Player.h"
#include ".\guiradarlayout.h"
#include "GuiSystem.h"

#include "AACamera.h"
#include "CommonDeclarations.h"
#include "Debugging.h"


const float ScreenSize = 0.25f; // in pixels
const int CompasFarDist = 20000;

GUIRadarLayout::GUIRadarLayout(const char *name): 
GUILayout(name),
AspectRatio(0),
CompasRect(NULL),
CompasObj(NULL),
//HaveCompasTarget(false),
TargetID(0),
WaitCompasTargetTimeout(0),
Active(false),
TargetType(CTT_NONE)
{
	Rows.Resize(CommonDeclarations::RadarObjectsCount);
	Objects.Resize(CommonDeclarations::RadarObjectsCount);
	memset(Rows.Data, 0, sizeof(Rows.Data)*Rows.Size);
	memset(Objects.Data, 0, sizeof(Objects.Data)*Objects.Size);
}

GUIRadarLayout::~GUIRadarLayout(void)
{
    for (size_t i=0;i<CommonDeclarations::RadarObjectsCount;++i)
    {	
		if (Rows.Data[i])
		{
			delete Rows.Data[i];
			Rows.Data[i] = NULL;
		}
		if (Objects.Data[i])
		{
			delete Objects.Data[i];
			Objects.Data[i] = NULL;
		}
    }
    if (CompasRect)
	{
        delete CompasRect;
		CompasRect = NULL;
	}
    if (CompasObj)
	{
        delete CompasObj;
		CompasObj = NULL;
	}
}

inline void GUIRadarLayout::DrawPos(unsigned num, Ogre::Vector3 dst_pos, Ogre::Quaternion players_orientation)
{
	Ogre::Rectangle2D *row = Rows.Data[num];
	Ogre::Rectangle2D *obj = Objects.Data[num];
	//dst_pos -= CommonDeclarations::MaxDistForRadar/2;
    Ogre::Quaternion orientation(Ogre::Degree(30), Ogre::Vector3::UNIT_X);
	Ogre::Quaternion player_rotation(players_orientation.getYaw(), Ogre::Vector3::UNIT_Y);
	player_rotation = Ogre::Quaternion(players_orientation.getRoll(), Ogre::Vector3::UNIT_Z) * player_rotation;
	
	Ogre::Vector3 rel_pos = dst_pos, base_pos = rel_pos;

	base_pos.y=0;

	float scr_height = rel_pos.y*ScreenSize / CommonDeclarations::MaxDistForRadar;

	base_pos = orientation * players_orientation * base_pos;
	base_pos = (base_pos / CommonDeclarations::MaxDistForRadar)*ScreenSize;

	base_pos.x += 0.75;
	base_pos.y -= 0.75;

	if (scr_height==0)
		scr_height=0.01;

	float left=base_pos.x-0.0035, top=base_pos.y, right=base_pos.x+0.0035, bottom=base_pos.y+scr_height;    

	//	
	obj->setCorners(base_pos.x-0.01, base_pos.y+0.005, base_pos.x+0.01, base_pos.y-0.005);
	if (scr_height>0)
	{       
		left=base_pos.x-0.0025;
		top=base_pos.y+scr_height;
		right=base_pos.x+0.0025;
		bottom=base_pos.y;
		obj->setMaterial("Radar/ObjMaterialUp");
	} else 
		if (scr_height<0)
		{   
			left=base_pos.x-0.0025;
			top=base_pos.y;
			right=base_pos.x+0.0025;
			bottom=base_pos.y+scr_height;
			obj->setMaterial("Radar/ObjMaterialDn");
		}
		row->setCorners(left, top, right, bottom);	
	
	row->setVisible(true);
	obj->setVisible(true);
}

inline void GUIRadarLayout::DrawCompas(Ogre::Quaternion players_orientation)
{
	//static clock_t last_update_time=0;
	//if (clock()-last_update_time<100)
	//	return;
	assert(CompasRect && CompasObj);
	//if (NULL==CompasRect || NULL==CompasObj)
	//	return;
	//last_update_time = clock();
    //CompasTarget;
	if (TargetID>0)
	{
		if (--WaitCompasTargetTimeout>=0)
		{
			CompasRect->setVisible(false);
			CompasObj->setVisible(false);
			return;
		}
		IAAObject *obj  = CommonDeclarations::GetIDObject(TargetID);
		//assert(obj);
		if (NULL==obj)
		{			
			RemoveCompasTarget();		
			return;
		}
		CompasTarget = obj->GetPosition();
	}
    Ogre::Rectangle2D *row = CompasRect;
    Ogre::Rectangle2D *obj = CompasObj;
    //dst_pos -= CommonDeclarations::MaxDistForRadar/2;
    Ogre::Quaternion orientation(Ogre::Degree(30), Ogre::Vector3::UNIT_X);
    Ogre::Quaternion player_rotation(players_orientation.getYaw(), Ogre::Vector3::UNIT_Y);
	player_rotation = Ogre::Quaternion(players_orientation.getRoll(), Ogre::Vector3::UNIT_Z) * player_rotation;

    Ogre::Vector3 pl_pos = CommonDeclarations::GetPlayer()->GetPosition();
    Ogre::Vector3 rel_pos = CompasTarget-pl_pos, base_pos = rel_pos;
	
	base_pos.y=0;

	bool checkpoint_mode=false;
    float scr_height = 0;
    if (rel_pos.squaredLength()>CommonDeclarations::SqMaxDistForRadar) 
    {
		Ogre::Vector3 dist=base_pos;
        AAUtilities::Norm1(dist);        
        base_pos = dist*CommonDeclarations::MaxDistForRadar;
        scr_height=0.01;		
		checkpoint_mode=true;
    } else
    {	
        scr_height =rel_pos.y*ScreenSize / CommonDeclarations::MaxDistForRadar;
    }

    base_pos = orientation * players_orientation * base_pos;
    base_pos = (base_pos / CommonDeclarations::MaxDistForRadar)*ScreenSize;

    base_pos.x += 0.75;
    base_pos.y -= 0.75;

    //	
	if (!checkpoint_mode)
	{
		if (scr_height==0)
			scr_height=0.01;
		float left=base_pos.x-0.005, top=base_pos.y, right=base_pos.x+0.005, bottom=base_pos.y+scr_height;    
		obj->setCorners(base_pos.x-0.01, base_pos.y+0.008, base_pos.x+0.01, base_pos.y-0.008);
		if (scr_height>0)
		{       
			left=base_pos.x-0.0025;
			top=base_pos.y+scr_height;
			right=base_pos.x+0.0025;
			bottom=base_pos.y;
			obj->setMaterial("Radar/CompasObjMaterialUp");
		} else 
			if (scr_height<0)
			{   
				left=base_pos.x-0.0025;
				top=base_pos.y;
				right=base_pos.x+0.0025;
				bottom=base_pos.y+scr_height;
				obj->setMaterial("Radar/CompasObjMaterialDn");
			}
			row->setCorners(left, top, right, bottom);	
			//		

			row->setVisible(true);
			obj->setVisible(true);
	} else
	{
		obj->setMaterial("Radar/CompasFarMaterial");
		obj->setCorners(base_pos.x-0.015, base_pos.y+0.015, base_pos.x+0.015, base_pos.y-0.015);		
		obj->setVisible(true);
	}
	
}

inline void  GUIRadarLayout::InitCompasRect(Ogre::Rectangle2D *&rect)
{	
    rect = new Ogre::Rectangle2D(TRUE);	
    rect->setMaterial("CompasMaterial");    
    rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_6);		
    //CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(rect);
	//rect->setVisible(false);
}

bool GUIRadarLayout::Load()
{
	//
	rect = new Ogre::Rectangle2D(TRUE);
	rect->setMaterial("Radar/Background");	
	rect->getMaterial()->setDepthCheckEnabled(false);
	rect->getMaterial()->setDepthWriteEnabled(false);	
	rect->setRenderQueueGroup(Ogre::RENDER_QUEUE_6-3);
	//rect->setBoundingBox(Ogre::AxisAlignedBox(-1000000 * Ogre::Vector3::UNIT_SCALE, 1000000 * Ogre::Vector3::UNIT_SCALE)); 
	//CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(rect); 
	//rect->setCorners(0.5, -0.5, 1, -1);
	//

	// Create a manual object for 2D	
	//char name[10];
	Ogre::Rectangle2D *row = NULL;
	Ogre::Rectangle2D *obj = NULL;
	//Ogre::AxisAlignedBox aabInf;
	//aabInf.setInfinite();
	
	if (!Ogre::MaterialManager::getSingleton().resourceExists("CompasMaterial"))
	{
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");
		CompasMaterial = mat->clone("CompasMaterial");
		CompasMaterial->setDepthCheckEnabled(false);
		CompasMaterial->setDepthWriteEnabled(false);
		CompasMaterial->setLightingEnabled(true);
		CompasMaterial->setAmbient(Ogre::ColourValue::Green);
	}

	if (!Ogre::MaterialManager::getSingleton().resourceExists("RadarRowMaterial"))
	{
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");
		RadarRowMaterial = mat->clone("RadarRowMaterial");
		RadarRowMaterial->setDepthCheckEnabled(false);
		RadarRowMaterial->setDepthWriteEnabled(false);
		RadarRowMaterial->setLightingEnabled(true);
		RadarRowMaterial->setAmbient(Ogre::ColourValue::Red);
	}

	/*if (!Ogre::MaterialManager::getSingleton().resourceExists("RadarObjMaterial"))
	{
		Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting");
		RadarObjMaterial = mat->clone("RadarObjMaterial");
		RadarObjMaterial->setDepthCheckEnabled(false);
		RadarObjMaterial->setDepthWriteEnabled(false);
		RadarObjMaterial->setLightingEnabled(true);
		RadarObjMaterial->setAmbient(Ogre::ColourValue::Red);
	}*/
	
	for (unsigned i=0;i<CommonDeclarations::RadarObjectsCount;++i)
	{		
		//
		row = new Ogre::Rectangle2D(TRUE);
		row->setMaterial("RadarRowMaterial");		
		row->setRenderQueueGroup(Ogre::RENDER_QUEUE_6-2);		
		//CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(row); 
		//rect->setCorners(0.5, -0.5, 1, -1);
		Rows.Data[i] = row;
		//

		//
		obj = new Ogre::Rectangle2D(TRUE);
		obj->setMaterial("Radar/ObjMaterialUp");
		obj->setRenderQueueGroup(Ogre::RENDER_QUEUE_6-2);		
		//CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(obj); 
		//rect->setCorners(0.5, -0.5, 1, -1);
		Objects.Data[i] = obj;
		//		
	}

    InitCompasRect(CompasRect);
    InitCompasRect(CompasObj);    
		
	return true;
}

bool GUIRadarLayout::Init()
{
	CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(rect); 
	rect->setCorners(0.5, -0.5, 1, -1);

	for (unsigned i=0;i<CommonDeclarations::RadarObjectsCount;++i)
	{		
		CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(Rows.Data[i]); 
		CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(Objects.Data[i]); 
	}
	
	CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(CompasRect);
	CompasRect->setVisible(false);
	
	CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(CompasObj);
	CompasObj->setVisible(false);

	AspectRatio = CommonDeclarations::GetCamera()->GetOgreCamera()->getAspectRatio();
	
	return true;
}

bool GUIRadarLayout::Update(CommonDeclarations::RadarObjectsArray &array)
{    
	if (!Active)
		return false;
	unsigned i=0;
	Ogre::Quaternion players_orientation = CommonDeclarations::GetPlayer()->GetOrientation().Inverse();
    for (i=0;i<array.Size && array.Data[i].x<CommonDeclarations::MaxDistForRadar;++i)
    {
        DrawPos(i, array.Data[i], players_orientation);
    }

	for (;i<array.Size;++i)
	{		
		Rows.Data[i]->setVisible(false);
		Objects.Data[i]->setVisible(false);
	}
    
    if (TargetType!=CTT_NONE)
    {
        DrawCompas(players_orientation);        
    }
    
	

    return true;
}

void GUIRadarLayout::Hide()
{
	for (size_t i=0;i<Rows.Size;++i)
	{
		Rows.Data[i]->setVisible(false);
		Objects.Data[i]->setVisible(false);
	}

	if (CompasRect)
		CompasRect->setVisible(false);
	if (CompasObj)
		CompasObj->setVisible(false);

	rect->setVisible(false);
	Active = false;
}

void GUIRadarLayout::Show(GUILayout *back_layou)
{
	rect->setVisible(true);
	Active = true;
}

void GUIRadarLayout::SetCompasTarget(Ogre::Vector3 pos)
{
    CommonDeclarations::SetAutoPilotActive(false);
    CompasTarget = pos;
    //HaveCompasTarget = true;
	TargetType = CTT_POINT;
}

void GUIRadarLayout::SetCompasTarget(int id, int wait_timeout)
{
    CommonDeclarations::SetAutoPilotActive(false);
	TargetID = id;
	WaitCompasTargetTimeout = wait_timeout;
	//HaveCompasTarget = true;
	TargetType = CTT_OBJECT;
}

void GUIRadarLayout::RemoveCompasTarget()
{
    CommonDeclarations::SetAutoPilotActive(false);
    //HaveCompasTarget = false;
	TargetType = CTT_NONE;
	TargetID = 0;
	if (CompasRect)
		CompasRect->setVisible(false);
	if (CompasObj)
		CompasObj->setVisible(false);
}

Ogre::Vector3 GUIRadarLayout::GetCompasTargetPoint() const
{
	return CompasTarget;
}

GUIRadarLayout::ECompasTargetType GUIRadarLayout::GetCompasTargetType() const
{
	return TargetType;
}

int GUIRadarLayout::GetCompasTargetID() const
{
	return TargetID;
}


#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Stall.h"
#include "CommonDeclarations.h"
#include "AACamera.h"

Stall::Stall():
Stalling(NULL),
Forestalling(NULL),
StallSize(0.1), 
ForestallSize(0.03),
AspectRatio(1)
{
}

Stall::~Stall()
{
    Close();    
}

void Stall::Close()
{
    if (Stalling)
	{
        delete Stalling;
		Stalling = NULL;
	}
    if (Forestalling)
	{
        delete Forestalling;
		Forestalling = NULL;
	}
}

void Stall::Load()
{
	//AspectRatio = CommonDeclarations::GetCamera()->GetOgreCamera()->getAspectRatio();
	if (NULL==Stalling)
	{
		Stalling = new Ogre::Rectangle2D(TRUE);	
		Stalling->setMaterial("StallingMaterial");    
		Stalling->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY-3);		
	}

	if (NULL==Forestalling)
	{
		Forestalling = new Ogre::Rectangle2D(TRUE);	
		Forestalling->setMaterial("ForestallingMaterial");    
		Forestalling->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY-3);		
	}
}

void Stall::Init()
{
	if (NULL!=Stalling)
	{		
		CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(Stalling);
		Stalling->setVisible(false);
	}

	if (NULL!=Forestalling)
	{
		CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(Forestalling);
		Forestalling->setVisible(false);
	}
	AspectRatio = CommonDeclarations::GetCamera()->GetOgreCamera()->getAspectRatio();
}

bool Stall::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	int res;
	res = xml->QueryFloatAttribute("stall_size", &StallSize);
	res = xml->QueryFloatAttribute("forestall_size", &ForestallSize);
	return true;
}

void Stall::SetPosition(Ogre::Rectangle2D *rect, Ogre::Vector3 pos, float size)
{    
    Ogre::Camera *camera =  CommonDeclarations::GetCamera()->GetOgreCamera();
    Ogre::Matrix4 vm = camera->getViewMatrix();
    Ogre::Matrix4 pm = camera->getProjectionMatrix();
    Ogre::Vector3 vpos = vm*pos;
    Ogre::Vector3 ppos = pm*vpos;

    rect->setCorners(ppos.x-size, ppos.y+size, ppos.x+size/AspectRatio, ppos.y-size);
}




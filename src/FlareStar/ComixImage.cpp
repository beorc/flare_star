#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "ComixImage.h"
#include "ScriptManager.h"
#include "CommonDeclarations.h"
#include "AACamera.h"
#include "AApplication.h"

ComixImage::ComixImage(void):
ImageRect(NULL),
CurrentTransparencyTrack(NULL),
Active(false),
ActionBeginTime(0),
ActionEndTime(9999999),
TimeInImage(0),
VisibleBeginTime(0),
VisibleEndTime(9999999)
{
}

ComixImage::~ComixImage(void)
{
	Close();
}

void ComixImage::Step(unsigned timeMs)
{    
    bool still_active=false;
	
	TimeInImage+=timeMs;

	if (TimeInImage>=VisibleBeginTime)
	{
		if (!ImageRect->isVisible())
		{
			Show();
		}
	}

	if (TimeInImage>=VisibleEndTime)
	{
		if (ImageRect->isVisible())
		{
			Hide();
		}
	}

	if (!Active)
		return;

	if (TimeInImage<ActionBeginTime)
		return;

    if (CurrentTransparencyTrack)
    {
		still_active = true;
		if (CurrentTransparencyTrack->Value.Active)
		{			
			Ogre::Pass *pass = ImageMaterial->getTechnique(0)->getPass(0);
			pass->setDiffuse(255,255,255,CurrentTransparencyTrack->Value.CurrentTransparency);
			if (pass->getNumTextureUnitStates()>0)
			{
				Ogre::TextureUnitState *texture_unit = ImageMaterial->getTechnique(0)->getPass(0)->getTextureUnitState(0);
				if (texture_unit)
					texture_unit->setAlphaOperation(Ogre::LBX_MODULATE, Ogre::LBS_MANUAL, Ogre::LBS_TEXTURE, CurrentTransparencyTrack->Value.CurrentTransparency);
			}			
			CurrentTransparencyTrack->Value.Step(timeMs);
		} else        
        {
            CurrentTransparencyTrack = CurrentTransparencyTrack->Next;
        }        
    }
    Active = still_active;
	
	if (TimeInImage>ActionEndTime)
	{
		Active = false;
	}
}

bool ComixImage::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened
	
    //const char *str;       
	
    //TiXmlElement *xml_element = 0;
    const char *str;	
		
    int res;    
    int ambient_r=0, ambient_g=0, ambient_b=0;
    int val;

    ImageRect = new Ogre::Rectangle2D(TRUE);

    str = xml->Attribute("material");
    ImageMaterial = Ogre::MaterialManager::getSingleton().getByName(str);

    ImageRect->setMaterial(str);
    
    CommonDeclarations::GetSceneManager()->getRootSceneNode()->attachObject(ImageRect);

    TiXmlElement *xml_element = NULL;
    xml_element = xml->FirstChildElement("rect");	
    if (xml_element)
    {
        float corner_left=0, corner_up=0, w=0, h=0;

		str = xml_element->Attribute("mode");
		//assert(str);

        res = xml_element->QueryFloatAttribute("x", &corner_left);
        assert(TIXML_SUCCESS == res);
        res = xml_element->QueryFloatAttribute("y", &corner_up);
        assert(TIXML_SUCCESS == res);
        res = xml_element->QueryFloatAttribute("w", &w);
        assert(TIXML_SUCCESS == res);
        res = xml_element->QueryFloatAttribute("h", &h);
        assert(TIXML_SUCCESS == res);

		//float AspectRatio = CommonDeclarations::GetCamera()->GetOgreCamera()->getAspectRatio();
		Ogre::Viewport *vp = CommonDeclarations::GetApplication()->GetCurrentViewPort();
		int vp_w = vp->getActualWidth(), vp_h = vp->getActualHeight();

		if (w>2)
		{
			w = w/vp_w;
			w *= 2;
		}

		if (h>2)
		{
			h = h/vp_h;
			h *= 2;
		}

		if (str && strcmp(str,"center")==0)
		{
			corner_left -= w/2;
			corner_up += h/2;
		}

        ImageRect->setCorners(corner_left, corner_up, corner_left+w, corner_up-h);        
    }

    xml_element = xml->FirstChildElement("transparency_tracks");	
    if (xml_element)
    {        
        xml_element = xml_element->FirstChildElement("track");
        while (xml_element)
        {
            TransparencyTrack track;
            track.Parse(xml_element);
            TransparencyTracks.PushBack(track);

            xml_element = xml_element->NextSiblingElement("track");
        }
        CurrentTransparencyTrack = TransparencyTracks.GetBegin();
    }

	xml_element = xml->FirstChildElement("action");	
	if (xml_element)
	{        
		res = xml_element->QueryIntAttribute("active", &val);
		//assert(TIXML_SUCCESS == res);

		if (TIXML_SUCCESS == res)
			Active = val ? true : false;

		res = xml_element->QueryIntAttribute("begin", &val);
		//assert(TIXML_SUCCESS == res);

		if (TIXML_SUCCESS == res)
			ActionBeginTime = (unsigned)val;

		res = xml_element->QueryIntAttribute("end", &val);
		//assert(TIXML_SUCCESS == res);

		if (TIXML_SUCCESS == res)
			ActionEndTime = (unsigned)val;		
	}

	xml_element = xml->FirstChildElement("visibility");	
	if (xml_element)
	{        
		res = xml_element->QueryIntAttribute("begin", &val);
		//assert(TIXML_SUCCESS == res);

		if (TIXML_SUCCESS == res)
			VisibleBeginTime = (unsigned)val;

		res = xml_element->QueryIntAttribute("end", &val);
		//assert(TIXML_SUCCESS == res);

		if (TIXML_SUCCESS == res)
			VisibleEndTime = (unsigned)val;
	}	
  	
    val = Ogre::RENDER_QUEUE_OVERLAY-3;
    res = xml->QueryIntAttribute("render_queue", &val);            
    ImageRect->setRenderQueueGroup(val);		
    
    ImageRect->setVisible(false);
        
    return false;
}

void ComixImage::Close()
{
		delete ImageRect;
        ImageMaterial.setNull();        
}

void ComixImage::Show()
{
	ImageRect->setVisible(true);
}

bool ComixImage::IsActive() const
{
    return Active;
}

void ComixImage::SetActive(bool on)
{
    Active = on;
}

void ComixImage::Hide()
{
    ImageRect->setVisible(false);
}

TransparencyTrack::TransparencyTrack() : 
CurrentTransparency(0.f),
MinTransparency(0.f),
MaxTransparency(0.f),
TransparencyStep(0.f),
Timing(0),
Active(true),
Mode(FM_NONE),
TimeInTrack(0),
FrameTimeout(100),
ActionBeginTime(0), 
ActionEndTime(9999999)
{
}

void TransparencyTrack::Step(unsigned timeMs)
{
    TimeInTrack+=timeMs;
	if (TimeInTrack<ActionBeginTime)
		return;
    switch(Mode) {
        case FM_FADEIN:
            {
                Timing+=timeMs;
                if ((int)Timing<FrameTimeout)
                    return;
                Timing = 0;

                if (CurrentTransparency>MinTransparency)
                {
                    CurrentTransparency-=TransparencyStep;
                    if (CurrentTransparency<MinTransparency)
                        CurrentTransparency=MinTransparency;                    
                } else
                {
                    Active = false;
                }  
                break;
            }        
        case FM_FADEOUT:
            {
                Timing+=timeMs;
                if ((int)Timing<FrameTimeout)
                    return;
                Timing = 0;

                if (CurrentTransparency<MaxTransparency)
                {
                    CurrentTransparency+=TransparencyStep;
                    if (CurrentTransparency>MaxTransparency)                
                        CurrentTransparency=MaxTransparency;                    
                } else
                {
                    Active = false;
                }
                break;
            }
    };
    if (TimeInTrack>ActionEndTime)
    {
        Active = false;
    }
}

bool TransparencyTrack::Parse(TiXmlElement *xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    int res;
    res = xml->QueryFloatAttribute("min", &MinTransparency);
    //assert(TIXML_SUCCESS == res);
    res = xml->QueryFloatAttribute("max", &MaxTransparency);
    //assert(TIXML_SUCCESS == res);
    res = xml->QueryFloatAttribute("current", &CurrentTransparency);
    //assert(TIXML_SUCCESS == res);

    int val=0;

	res = xml->QueryIntAttribute("action_begin", &val);
	//assert(TIXML_SUCCESS == res);

	if (TIXML_SUCCESS == res)
		ActionBeginTime = (unsigned)val;

    res = xml->QueryIntAttribute("action_end", &val);
    //assert(TIXML_SUCCESS == res);

	if (TIXML_SUCCESS == res)
		ActionEndTime = (unsigned)val;

	res = xml->QueryIntAttribute("frame_timeout", &FrameTimeout);
	//assert(TIXML_SUCCESS == res);
	
    val=FM_NONE;
    res = xml->QueryIntAttribute("mode", &val);
    assert(TIXML_SUCCESS == res);
    Mode = (TransparencyTrack::EFadeMode)val;

	float trnsparency_range=0.f;

	switch(Mode) {
	case FM_FADEIN:		
		trnsparency_range = CurrentTransparency - MinTransparency;
		break;
	case FM_FADEOUT:
		trnsparency_range = MaxTransparency - CurrentTransparency;
		break;	
	};
	
	res = xml->QueryIntAttribute("track_time", &val);
	assert(TIXML_SUCCESS == res);

	val = val/FrameTimeout; // number of frames
	TransparencyStep = trnsparency_range/val;

    return true;
}




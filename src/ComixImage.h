#pragma once

#include "List.h"

class TiXmlElement;

struct TransparencyTrack
{
    enum EFadeMode {FM_NONE, FM_FADEIN, FM_FADEOUT};
    TransparencyTrack();

    void Step(unsigned timeMs);
    bool Parse(TiXmlElement *xml);    

    float MinTransparency, MaxTransparency, TransparencyStep;
    float CurrentTransparency;
    unsigned Timing, TimeInTrack;
	int FrameTimeout;
    bool Active;
    EFadeMode Mode;
	unsigned ActionBeginTime, ActionEndTime;
};

class ComixImage {
public:    
    ComixImage(void);
    ~ComixImage(void);
    
    void Step(unsigned timeMs);
    bool Parse(TiXmlElement *xml);
    void Close();
    void Show();
    void Hide();

    bool IsActive() const;
    void SetActive(bool);
protected:
    Ogre::MaterialPtr ImageMaterial;
    Ogre::Rectangle2D *ImageRect;

    List<TransparencyTrack> TransparencyTracks;        
    List<TransparencyTrack>::ListNode *CurrentTransparencyTrack;
    bool Active;
	int ActionBeginTime, ActionEndTime, TimeInImage, VisibleBeginTime, VisibleEndTime;
};

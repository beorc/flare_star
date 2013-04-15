#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "MasterEffect.h"
#include "BillboardSubEffect.h"
#include "MeshSubEffect.h"

#include "CommonDeclarations.h"
#include "Utilities.h"
#include "ObjectsLibrary.h"
#include <OgreAL.h>
#include "IScenable.h"
#include "ISubEffect.h"
#include "Configuration.h"

MasterEffect::MasterEffect(void) :
RelativePos(0,0,0),
Parent(NULL),
Duration(-1),
Name(NULL),
Type(IMasterEffect::MET_NONE),
Running(false),
RelOrientation(Ogre::Quaternion::IDENTITY),
LastTime(0),
LID(0),
RunOnCreate(false),
ParentNode(NULL),
IsLoop(false),
EffectNode(NULL)
{
}

MasterEffect::~MasterEffect(void)
{
	if (Name)
		delete [] Name;
    
    if (SoundFiles.get())
    {
        for (List<char*>::ListNode *pos = SoundFiles->GetBegin();pos!=NULL;pos=pos->Next)
            delete [] pos->Value;	
    }

    if (Sounds.get())
    {
        OgreAL::SoundManager *manager = CommonDeclarations::GetSoundManager();
		SoundsContainer::iterator iPos = Sounds->begin(), iEnd = Sounds->end();
        for (;iPos!=iEnd;++iPos)
        {
            (*iPos)->stop();
            manager->destroySound(*iPos);
        }
    }

    ISubEffect *effect;        
    SubEffectsContainer::iterator iPos = SubEffects.begin(), iEnd = SubEffects.end();
    for (;iPos!=iEnd;++iPos)
    {
        effect = *iPos;
        effect->Destroy();
    }
    SubEffects.clear();
}

bool MasterEffect::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;

	xml = ObjectsLibrary::CheckLID(xml);

	str = xml->Attribute("type");

	if (str)
	{
		if (strcmp(str,"onshoot")==0)
			Type=IMasterEffect::MET_ONSHOOT;
		else
			if (strcmp(str,"onblow")==0)
				Type=IMasterEffect::MET_ONBLOW;
			else
				if (strcmp(str,"permanent")==0)
					Type=IMasterEffect::MET_PERMANENT;
	}
	
	int x=0, y=0, z=0, res, resx, resy, resz,val;
	resx = xml->QueryIntAttribute("x", &x);	
	resy = xml->QueryIntAttribute("y", &y);	
	resz = xml->QueryIntAttribute("z", &z);	

	if (TIXML_SUCCESS==resx && TIXML_SUCCESS==resy && TIXML_SUCCESS==resz) 
	{
		RelativePos = Ogre::Vector3((float)x,(float)y,(float)z);
	}

	float dirx, diry, dirz;
	resx = xml->QueryFloatAttribute("dirx", &dirx);	
	resy = xml->QueryFloatAttribute("diry", &diry);	
	resz = xml->QueryFloatAttribute("dirz", &dirz);	

	if (TIXML_SUCCESS==resx && TIXML_SUCCESS==resy && TIXML_SUCCESS==resz)
	{
		Ogre::Vector3 relative_dir(dirx,diry,dirz);
		RelOrientation = relative_dir.getRotationTo(Ogre::Vector3::UNIT_Z);
	}
	    	
	str = xml->Attribute("name");
	assert(str);
	if (Name)
		delete [] Name;
    Name = AAUtilities::StringCopy(str);

	res = xml->QueryIntAttribute("duration", &Duration);
	
	val=0;
	res = xml->QueryIntAttribute("run", &val);        
	RunOnCreate = val ? true : false;

    val=0;
    res = xml->QueryIntAttribute("loop", &val);        
    IsLoop = val ? true : false;
    
    TiXmlElement *xml_element = 0;
	xml_element = xml->FirstChildElement("subeffects");
	
	if (xml_element)
	{
        int val=0, res=0;
        		
		ISubEffect *effect;		
		xml_element = xml_element->FirstChildElement("subeffect");	
		
		while (xml_element)
		{				            
            str = xml_element->Attribute("class");
            assert(str);
            if (strcmp(str,"MeshSubEffect")==0)
            {
                effect = new MeshSubEffect();
            } else
                if (strcmp(str,"BillboardSubEffect")==0)
                {
                    effect = new BillboardSubEffect();
                }

            assert(effect);
                        
            if (effect) 
            {         
				effect->SetMaster(this);
                effect->Parse(xml_element);
                SubEffects.push_back(effect);
            }
			xml_element = xml_element->NextSiblingElement("subeffect");
		}         
	}

    xml_element = xml->FirstChildElement("sounds");

    if (xml_element)
    {
		SoundQueues.reset(new std::list<SSoundDescription>);
		TiXmlElement *xml_sound_element = 0;
		xml_sound_element = xml_element->FirstChildElement("sound");	
        while (xml_sound_element)
        {			
			SSoundDescription descr;
			val = 0;
            res = xml_sound_element->QueryIntAttribute("queue", &val);
			descr.QueueNumber = val;

			val = 1;
			res = xml_sound_element->QueryIntAttribute("priority", &val);
			descr.Priority = val;

			SoundQueues->push_back(descr);
            xml_sound_element = xml_sound_element->NextSiblingElement("sound");
        }

        List<char*>* res = AAUtilities::ParseStrings(xml_element, "sound", "file");
        if (res)
            SoundFiles.reset(res);
    }     
	
	return true;
}

void MasterEffect::Step(unsigned timeMs)
{
    if (Type!=MET_PERMANENT)
    {
        LastTime+=timeMs;
        if (Running && LastTime>=(unsigned)Duration) //need stop effect
        {
            LastTime = 0;
            Stop();
            if (IsLoop)
                Start();
        }
    }

    if (Running) //do effect
    {		
        //do effect
        ISubEffect *effect;        
        SubEffectsContainer::iterator iPos = SubEffects.begin(), iEnd = SubEffects.end();
		for (;iPos!=iEnd;++iPos)
		{
			effect = *iPos;
            effect->Step(timeMs);
        }
    }

    return;
}

bool MasterEffect::Start()
{
	Running=true;

	ISubEffect *effect;	
	SubEffectsContainer::iterator iPos = SubEffects.begin(), iEnd = SubEffects.end();
    for (;iPos!=iEnd;++iPos)
    {
        effect = *iPos;
		effect->Start();
	}

    if (Sounds.get())
    {
        SoundsContainer::iterator iPos = Sounds->begin(), iEnd = Sounds->end();
        for (;iPos!=iEnd;++iPos)
        {         
            (*iPos)->rewind();
            (*iPos)->play();        
        }
    }

	return false;
}

bool MasterEffect::Stop()
{
    Running = false;
	LastTime=0;

	ISubEffect *effect;	
	SubEffectsContainer::iterator iPos = SubEffects.begin(), iEnd = SubEffects.end();
    for (;iPos!=iEnd;++iPos)
    {
        effect = *iPos;
		effect->Stop();
	}

    return false;
}

void MasterEffect::SetParent(IScenable *parent)
{
	Parent = parent;
	if (parent)
		SetParentNode(parent->GetNode());
	else
	{
		ParentNode = NULL;
		EffectNode = NULL;
	}
}

void MasterEffect::SetParentNode(Ogre::SceneNode *parent_node)
{
	ParentNode = parent_node;
	char *buffer = CommonDeclarations::GenGUID();
	EffectNode = ParentNode->createChildSceneNode(buffer, RelativePos, RelOrientation);
	delete [] buffer;	
}

const char *MasterEffect::GetName()
{
	return Name;
}

IMasterEffect::EMasterEffectType MasterEffect::GetType()
{
	return Type;
}

void MasterEffect::SetType(IMasterEffect::EMasterEffectType type)
{
	Type=type;
}

void MasterEffect::SetRelativePosition(Ogre::Vector3 pos)
{
	RelativePos = pos;	
}

void MasterEffect::SetRelativeOrientation(Ogre::Quaternion or)
{
    RelOrientation = or;
}

void MasterEffect::SetDuration(int duration)
{
	Duration = duration;
}

int MasterEffect::GetLID() const
{
	return LID;
}

void MasterEffect::SetLID(int lid)
{
	LID = lid;
}

bool MasterEffect::Init()
{
	EffectNode->setPosition(RelativePos);
	EffectNode->setOrientation(RelOrientation);

	ISubEffect *effect;	
	SubEffectsContainer::iterator iPos = SubEffects.begin(), iEnd = SubEffects.end();
    for (;iPos!=iEnd;++iPos)
    {
        effect = *iPos;
		effect->Init();
	}

	InitSounds();

	if (RunOnCreate)
		Start();
	return true;
}

IScenable *MasterEffect::GetParent() const
{
    return Parent;
}

bool MasterEffect::InitSounds()
{
    if (Sounds.get())
        return false;
    OgreAL::SoundManager *SoundManager = CommonDeclarations::GetSoundManager();
	SoundsContainer *sounds = new SoundsContainer;
    Sounds.reset(sounds);
    OgreAL::Sound *sound=NULL;
    char *file;    

	
    if (SoundFiles.get() && !SoundFiles->IsEmpty())
    {        
        sounds->resize(SoundFiles->GetSize());
        
		size_t i=0;
		
		std::list<SSoundDescription>::iterator iPos = SoundQueues->begin();
		for (List<char*>::ListNode *pos = SoundFiles->GetBegin();pos!=NULL;pos=pos->Next, ++iPos)
        {
            file = pos->Value;
            char *buffer = CommonDeclarations::GenGUID();

            sound =  SoundManager->createSound(buffer, file, false, false);
            delete [] buffer;
			
			assert(ParentNode);
            if (ParentNode)
                ParentNode->attachObject(sound);
            
			sound->setGain(Configuration::GetInstance()->GetEffectsVolume());
			sound->SetQueueNumber(iPos->QueueNumber);
			sound->setPriority((OgreAL::Sound::Priority)iPos->Priority);
						
			(*sounds)[i]=sound;
        }
    }
    
    return true;
}

Ogre::SceneNode *MasterEffect::GetEffectNode() const
{
	return EffectNode;
}



#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Bonus.h"
#include "BonusesKeeper.h"
#include "CommonDeclarations.h"
#include "IAAObject.h"


Bonus::Bonus() :
SlotType(0),
Owner(NULL),
Storable(false),
Timeout(0),
ActiveTime(0),
DescriptableID(0)
{
}

Bonus::~Bonus(void)
{
}

void Bonus::SetOwner(BonusesKeeper *owner)
{
	Owner = owner;
}

int Bonus::GetSlotType()
{
	return SlotType;
}

bool Bonus::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	int res, val;
	res = xml->QueryIntAttribute("slottype", &SlotType);	
	assert(TIXML_SUCCESS==res);

	res = xml->QueryIntAttribute("storable", &val);	
	//assert(TIXML_SUCCESS==res);
	if (TIXML_SUCCESS==res)
		Storable = val ? true : false;

	res = xml->QueryIntAttribute("time", &Timeout);	
	
	TiXmlElement *xml_element=NULL;
	xml_element = xml->FirstChildElement("modifiers");
    
	if (xml_element)
	{
		Modifiers.Parse(xml_element);
	}

	xml_element = xml->FirstChildElement("description");
	if (xml_element) 
	{		
		xml_element->QueryIntAttribute("id", &DescriptableID);
		//DescriptableReference = Descriptable::GetDescription(val);
	}

	return true;
}

bool Bonus::ApplyIn(IAAObject *obj)
{
	if (Modifiers.SpeedMult!=1)
	{
		IPhysical *phys = obj->GetPhysical();
		if (phys)
		{
			Ogre::Vector3 curr_mv = phys->GetMaxVelocities();
			phys->SetMaxVelocities(curr_mv*Modifiers.SpeedMult);
		}
	}
	return false;
}

void Bonus::Step( unsigned timeMs )
{
	ActiveTime+=timeMs;
	if (ActiveTime>Timeout)
	{
		if (Modifiers.SpeedMult!=1)
		{
			IPhysical *phys = Owner->GetOwner()->GetPhysical();
			if (phys)
			{
				Ogre::Vector3 curr_mv = phys->GetMaxVelocities();
				phys->SetMaxVelocities(curr_mv/Modifiers.SpeedMult);
			}
		}
		Owner->RemoveBonus(this);
	}	
}

bool Bonus::IsStorable() const
{
	return Storable;
}

int Bonus::GetDescriptableID() const
{
	return DescriptableID;
}

Bonus::SModifiers::SModifiers() :
SpeedMult(1.f)
{
}

bool Bonus::SModifiers::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	int res;
	res = xml->QueryFloatAttribute("speedmult", &SpeedMult);	
	//assert(TIXML_SUCCESS==res);
	return true;
}





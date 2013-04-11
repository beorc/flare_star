#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "BonusesKeeper.h"
#include "ObjectsLibrary.h"
#include "Bonus.h"
#include "IAAObject.h"
#include "HUDCentre.h"


BonusesKeeper::BonusesKeeper(void)
{
}

BonusesKeeper::~BonusesKeeper(void)
{	
	Clear();	
}

bool BonusesKeeper::AddBonus(Bonus* bonus)
{
	bool res = bonus->ApplyIn(Owner);
	if (!bonus->IsStorable())
	{		
		delete bonus;
		return res;
	}
	//SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	int slot_type = bonus->GetSlotType();
	for (SlotsPool::ListNode *pos = Slots.GetBegin();pos!=NULL;pos=pos->Next)
	{
		//SBonusSlot slot = pos->Value;
		if ((pos->Value.Type==slot_type || BonusesKeeper::SBonusSlot::UNIVERSAL_SLOT==slot_type) && pos->Value.ChildBonus==NULL)
		{
            pos->Value.ChildBonus=bonus;			
			bonus->SetOwner(this);
			if (Owner->GetTag()==T_PLAYER) 
			{
				HUDCentre::GetInstance()->AddBonus(bonus->GetDescriptableID());
			}
			return true;
		}
	}
	return false;		
}

bool BonusesKeeper::RemoveBonus(Bonus* bonus)
{
	for (SlotsPool::ListNode *pos = Slots.GetBegin();pos!=NULL;pos=pos->Next)
	{		
		if (pos->Value.ChildBonus==bonus)
		{
			if (Owner->GetTag()==T_PLAYER)
				HUDCentre::GetInstance()->RemoveBonus(pos->Value.ChildBonus->GetDescriptableID());
			pos->Value.ChildBonus = NULL;			
			delete bonus;
			return true;
		}
	}
	return false;
}

void BonusesKeeper::Step( unsigned timeMs )
{	
	for (SlotsPool::ListNode *pos = Slots.GetBegin();pos!=NULL;pos=pos->Next)
	{
		//SBonusSlot slot = pos->Value;
		if (pos->Value.ChildBonus!=NULL)
		{
			pos->Value.ChildBonus->Step(timeMs);
		}
	}
}

bool BonusesKeeper::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	int res=0;
	TiXmlElement *xml_element = 0;
	xml_element = xml->FirstChildElement("slots");
	if (xml_element) 
	{				
		xml_element = xml_element->FirstChildElement("slot");
		while(xml_element)
		{			
			double x=0,y=0,z=0;				
			SBonusSlot slot;
			res = xml_element->QueryIntAttribute("type", &slot.Type);
			assert(TIXML_SUCCESS==res);

			slot.ChildBonus = NULL;

			Slots.PushBack(slot);
			xml_element = xml_element->NextSiblingElement();
		}
	}
	
	SBonusDescription bonus;
	xml_element = xml->FirstChildElement("bonuses");
	if (xml_element) 
	{
		int val;
		xml_element = xml_element->FirstChildElement("bonus");
		while(xml_element)
		{
			res = xml_element->QueryIntAttribute("lid", &bonus.LID);
			assert(TIXML_SUCCESS==res);
			
			res = xml_element->QueryIntAttribute("drop_prob", &val);			
			if (TIXML_SUCCESS==res)
				bonus.DropProbability = (char)val;
			else
				bonus.DropProbability=0;

			res = xml_element->QueryIntAttribute("active", &bonus.Active);
			assert(TIXML_SUCCESS==res);
			
			bonus.Inited = false;
			Bonuses.PushBack(bonus);
			xml_element = xml_element->NextSiblingElement();
		}
	}

	return true;
}

//void BonusesKeeper::Init()
//{
//	Bonus *obj;
//	bool res;
//	if (Slots.IsEmpty())
//		return;
//	for (BonusesContainer::ListNode *pos = Bonuses.GetBegin();pos!=NULL;pos=pos->Next)
//	{
//		SBonusDescription descr = pos->Value;
//		if (descr.Active)
//		{
//			obj = ObjectsLibrary::GetInstance()->LoadBonus(pos->Value.LID);
//			assert(obj);			
//			if (obj)
//			{
//				if (!obj->IsStorable())
//				{
//					bool res = bonus->ApplyIn(Owner);
//					delete bonus;
//					return res;
//				}
//				pos->Value.Inited = res;
//			}
//		}				
//	}
//}

void BonusesKeeper::SetOwner(IAAObject *owner)
{
	Owner = owner;
}

IAAObject *BonusesKeeper::GetOwner()
{
	return Owner;
}

BonusesKeeper::SlotsPool *BonusesKeeper::GetSlots()
{
	return &Slots;
}

void BonusesKeeper::Drop(bool prob_mode)
{
	IAAObject *obj;
	for (BonusesContainer::ListNode *pos = Bonuses.GetBegin();pos!=NULL;pos=pos->Next)
	{
		int exper = AAUtilities::Rand()*100.f;
		if (!prob_mode || pos->Value.DropProbability>exper)
		{
			int radius = Owner->GetPhysical()->GetRadius();
			obj = ObjectsLibrary::GetInstance()->Load(pos->Value.LID);		
			Ogre::Vector3 ipos = Owner->GetPosition()+Ogre::Vector3::NEGATIVE_UNIT_Y*(radius+1000)+Ogre::Vector3::NEGATIVE_UNIT_Z*radius*AAUtilities::Rand(); //-Owner->GetPhysical()->GetLinVelocity()*10
			obj->GetScenable()->SetInitialPosition(ipos);
			//obj->GetScenable()->GetNode()->setPosition(ipos);
			CommonDeclarations::AddObjectRequest(obj);
		}
	}
	Bonuses.Clear();
	Clear();
}

void BonusesKeeper::Clear()
{	
	for (SlotsPool::ListNode *pos = Slots.GetBegin();pos!=NULL;pos=pos->Next)
	{
		//SBonusSlot slot = pos->Value;
		if (pos->Value.ChildBonus!=NULL)
		{
			delete pos->Value.ChildBonus;
			pos->Value.ChildBonus=NULL;			
		}
	}
}

bool BonusesKeeper::TestSlot(int type)
{
	for (SlotsPool::ListNode *pos = Slots.GetBegin();pos!=NULL;pos=pos->Next)
	{
		//SBonusSlot slot = pos->Value;
		if (pos->Value.ChildBonus==NULL && (pos->Value.Type==type || BonusesKeeper::SBonusSlot::UNIVERSAL_SLOT==type))
		{
			return true;
		}
	}
	return false;
}


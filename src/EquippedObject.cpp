#include "StdAfx.h"
//#include <crtdbg.h>
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\equippedobject.h"
#include "GenericWeapon.h"
#include "Container.h"
#include "ObjectsLibrary.h"
#include "HUDCentre.h"


EquippedObject::EquippedObject(void) :
Shooting(false),
AmmoVelocity(0),
RelativeSightOrigin(Ogre::Vector3::ZERO)
{
}

EquippedObject::EquippedObject(SceneObject *owner) :
Shooting(false)
{
	SetOwner(owner);
}

EquippedObject::~EquippedObject(void)
{	
	Clear();	
}

bool EquippedObject::AddEquipment(Equipment* equipment)
{
	//equipment->Init();

	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	int slot_type = equipment->GetSlotType();
	for (;iPos!=iEnd;++iPos)
	{
		if (iPos->Type==slot_type && iPos->ChildEquipment==NULL)
		{
            iPos->ChildEquipment=equipment;			
			equipment->SetRelativePos(iPos->RelativePos);
			equipment->SetOwner(this);
			if (equipment->GetType()==Weapon::ET_WEAPON)
			{
				Weapon *weapon = (Weapon*)equipment;
				AmmoVelocity = weapon->GetAmmoVelocity();
			}			
			CalculateSightOrigin();			
			return true;
		}
	}
	return false;		
}

//bool EquippedObject::TestToAdd(int slot_type) const
//{
//	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
//	int slot_type = equipment->GetSlotType();
//	for (;iPos!=iEnd;++iPos)
//	{
//		if (iPos->Type==slot_type && iPos->ChildEquipment==NULL)
//		{			
//			return true;
//		}
//	}
//	return false;	
//}


void EquippedObject::Step( unsigned timeMs )
{
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();	
	for (;iPos!=iEnd;++iPos)
	{
		if (iPos->ChildEquipment!=NULL)
		{
			iPos->ChildEquipment->Step(timeMs);
		}
	}
}

void EquippedObject::AddBonus(IEquipped::SBonus bonus)
{
    Bonuses.PushBack(bonus);
    if (Owner->GetTag()==T_PLAYER) 
    {
        HUDCentre::GetInstance()->AddEqupment(bonus.DescriptableID);
    }
}

bool EquippedObject::Parse(TiXmlElement* xml)
{
	assert(xml);
	if (xml == 0)
		return false; // file could not be opened

	const char *str;	
	str = xml->Attribute("cfg");
	int res;

	if (str)
	{
		return XMLConfigurable::Parse(str);
	}

	//SceneObject::Parse(xml);		

	str = xml->Value();
	assert(strcmp(str,"equipment")==0);

	TiXmlElement *xml_element = 0;
	xml_element = xml->FirstChildElement("slots");
	if (xml_element) 
	{				
		xml_element = xml_element->FirstChildElement("eqslot");
		while(xml_element)
		{			
			double x=0,y=0,z=0;				
			SSlot slot;
			res = xml_element->QueryIntAttribute("slottype", &slot.Type);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryDoubleAttribute("x", &x);			
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryDoubleAttribute("y", &y);			
			assert(TIXML_SUCCESS==res);
			res = xml_element->QueryDoubleAttribute("z", &z);			
			assert(TIXML_SUCCESS==res);

			slot.RelativePos = Vector3(x,y,z);
			slot.ChildEquipment = NULL;

			Slots.push_back(slot);
			xml_element = xml_element->NextSiblingElement();
		}
	}
	
	IEquipped::SBonus bonus;
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
			
			bonus.Inited = false;
			Bonuses.PushBack(bonus);
			xml_element = xml_element->NextSiblingElement();
		}
	}

	

	/*
	xml_element = xml->FirstChildElement("slotcontents");
	if (xml_element) 
	{
		xml_element = xml_element->FirstChildElement("weapon");
		while(xml_element)
		{
			const char *weaponclassname = xml_element->Attribute("class");		
			if (strcmp("GenericWeapon",weaponclassname)==0) 
			{			
				GenericWeapon *ret = new GenericWeapon();
				if (!ret->Parse(xml_element))
				{
					assert(false);
					continue;
				}			
				AddEquipment(ret);
			}
			xml_element = xml_element->NextSiblingElement();
		}
	}
	*/
	
	return true;
}

void EquippedObject::SaveTo(TiXmlElement * root) const
{
	TiXmlElement *elem = new TiXmlElement( "equipment" ), *slots, *slotcontents = NULL, *eqslot;  
	root->LinkEndChild( elem );  

	slots = new TiXmlElement( "slots" );  
	elem->LinkEndChild( slots );
	
	slotcontents = new TiXmlElement( "bonuses" );  
	elem->LinkEndChild( slotcontents );
	
	for (IEquipped::BonusesContainer::ListNode *pos = Bonuses.GetBegin();pos!=NULL;pos=pos->Next)
	{
		eqslot = new TiXmlElement( "bonus" );  
		slots->LinkEndChild( eqslot );
		
		eqslot->SetAttribute("lid",pos->Value.LID);		
	}

	/*slotcontents = new TiXmlElement( "slotcontents" );  
	elem->LinkEndChild( slotcontents );

	SlotsPool::const_iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		eqslot = new TiXmlElement( "eqslot" );  
		slots->LinkEndChild( eqslot );

		eqslot->SetAttribute("slottype",iPos->Type.c_str());
		eqslot->SetAttribute("x",iPos->RelativePos.x);
		eqslot->SetAttribute("y",iPos->RelativePos.y);
		eqslot->SetAttribute("z",iPos->RelativePos.z);

		if (iPos->ChildEquipment)
		{
			iPos->ChildEquipment->SaveTo(slotcontents);            
		}
	}
	*/
}

void EquippedObject::Init()
{
	IAAObject *obj;
	bool res;
	if (Slots.empty())
		return;
	for (IEquipped::BonusesContainer::ListNode *pos = Bonuses.GetBegin();pos!=NULL;pos=pos->Next)
	{
		obj = ObjectsLibrary::GetInstance()->Load(pos->Value.LID);
		assert(obj);
		if (obj)
		{
			res = ((Container*)obj)->ApplyIn(Owner);
			pos->Value.Inited = res;
		}		
	}

	//Drop();
	
	/*Equipment *equipment;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	
	for (;iPos!=iEnd;++iPos)
	{
		equipment=iPos->ChildEquipment;
			
		if (equipment!=NULL)
		{
			equipment->SetParent(Owner);
			equipment->Init();			
		}
	}
	*/	
}

void EquippedObject::SetOwner(SceneObject *owner)
{
	Owner = owner;
}

IAAObject *EquippedObject::GetOwner()
{
	return Owner;
}

EquippedObject::SlotsPool *EquippedObject::GetSlots()
{
	return &Slots;
}

Equipment *EquippedObject::GetFirstWeapon()
{
	Equipment *equipment;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{			
			return equipment;
		}
	}
	return NULL;
}

void EquippedObject::SetShooting(bool state)
{
	if (Shooting && state)
		return;
	Shooting = state;
	Equipment *equipment;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{	
			switch(state) {
				case true:
					((Weapon*)equipment)->SetShootingMode(Weapon::SM_SHOOTING);
					break;
				case false:
					((Weapon*)equipment)->SetShootingMode(Weapon::SM_NOTSHOOTING);
					break;	
			}
		}
	}	
}

void EquippedObject::SetTargetPosition(Ogre::Vector3 pos)
{
	Equipment *equipment;

	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{	
			((Weapon*)equipment)->SetTargetPosition(pos);
		}
	}
}

void EquippedObject::SetTargetPosition(Ogre::Vector3 pos, int distance)
{
	Equipment *equipment;
	Ogre::Ray ray = GetSightRay();
	Ogre::Vector3 focus;

	focus = ray.getOrigin() + ray.getDirection()*distance;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{	
			((Weapon*)equipment)->SetTargetPosition(focus);
		}
	}
}

Ogre::Ray EquippedObject::GetSightRay()
{
	//unsigned count=0;
	//Ogre::Vector3 origin(Ogre::Vector3::ZERO);
	////Origin = Ogre::Vector3::ZERO;
	//Equipment *equipment;
	//SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	//for (;iPos!=iEnd;++iPos)
	//{
	//	equipment = iPos->ChildEquipment;
	//	if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
	//	{	
	//		origin += ((Weapon*)equipment)->GetNode()->getWorldPosition();
	//		++count;
	//	}
	//}
	//if (count>0)
	//{
	//	origin = origin/count;
	//}

	Ogre::Quaternion orientation = Owner->GetOrientation();

	Ogre::Vector3 origin = Owner->GetPosition() + orientation*RelativeSightOrigin;

	Ogre::Ray ray(origin, orientation*Ogre::Vector3::NEGATIVE_UNIT_Z);
	return ray;
}

void EquippedObject::Drop(bool prob_mode)
{    
	IAAObject *obj;
	for (IEquipped::BonusesContainer::ListNode *pos = Bonuses.GetBegin();pos!=NULL;pos=pos->Next)
	{
		int exper = (rand()/(RAND_MAX+1.0))*100.f;
		if (!prob_mode || pos->Value.DropProbability>exper)
		{
			obj = ObjectsLibrary::GetInstance()->Load(pos->Value.LID);		
			Ogre::Vector3 ipos = Owner->GetPosition()+Ogre::Vector3::NEGATIVE_UNIT_Y*(Owner->GetPhysical()->GetRadius()+1000); //-Owner->GetPhysical()->GetLinVelocity()*10
			obj->GetScenable()->SetInitialPosition(ipos);
			//obj->GetScenable()->GetNode()->setPosition(ipos);
            if (Owner->GetTag()==T_PLAYER)
                HUDCentre::GetInstance()->RemoveEqupment(pos->Value.DescriptableID);
			CommonDeclarations::AddObjectRequest(obj);			
		}
	}
	Bonuses.Clear();
	Clear();
}

void EquippedObject::Clear()
{
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		if (iPos->ChildEquipment!=NULL)
		{			
			iPos->ChildEquipment->Destroy();
			iPos->ChildEquipment = NULL;			
		}
	}	
}

IEquipped::BonusesContainer *EquippedObject::GetBonuses()
{
	return &Bonuses;
}

Ogre::Vector3 EquippedObject::CalculateTargetPosition(Ogre::Vector3 pos, Ogre::Vector3 velocity)
{
	//velocity.normalise();	
	Ogre::Vector3 dist = pos - Owner->GetPosition();
	int time2fly = (dist.length()/AmmoVelocity);
	Ogre::Vector3 target = pos + velocity*time2fly;
	return target;
}

bool EquippedObject::CheckWeaponToShoot(Ogre::Vector3 tpos, int tradius)
{	
	Equipment *equipment;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	bool success = false;
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{				
			success = ((Weapon*)equipment)->TryToShoot(tpos, tradius);			
		}
	}	
	return success;
}

bool EquippedObject::TryToShoot(Ogre::Vector3 tpos, int tradius)
{
	std::pair<bool, Ogre::Real> intersection_res;
	
	Ogre::Ray pl_ray = GetSightRay();
	
	//Ogre::Vector3 tpos = CalculateTargetPosition(obj->GetPosition(), phys->GetLastVelocity());
	int tbox_size = tradius;

	Ogre::AxisAlignedBox tbox(tpos.x-tbox_size,tpos.y-tbox_size,tpos.z-tbox_size,tpos.x+tbox_size,tpos.y+tbox_size,tpos.z+tbox_size);

	intersection_res = pl_ray.intersects(tbox);
	if (intersection_res.first)
	{
		SetTargetPosition(tpos, intersection_res.second);
		SetShooting(true);
	} else
	{
		if (CheckWeaponToShoot(tpos, tradius)) {
			SetShooting(true);
		} else
		{
			SetTargetPosition(Ogre::Vector3::ZERO);
			SetShooting(false);
		}		
	}
	
	return true;
}

void EquippedObject::CalculateSightOrigin()
{
	unsigned count=0;
	Ogre::Vector3 origin(Ogre::Vector3::ZERO);
	//Origin = Ogre::Vector3::ZERO;
	Equipment *equipment;
	SlotsPool::iterator iPos = Slots.begin(), iEnd = Slots.end();
	for (;iPos!=iEnd;++iPos)
	{
		equipment = iPos->ChildEquipment;
		if (equipment!=NULL && equipment->GetType()==Weapon::ET_WEAPON)
		{	
			origin += equipment->GetRelativePos();
			++count;
		}
	}
	if (count>0)
	{
		RelativeSightOrigin = origin/count;		
	}
}

Ogre::Vector3 EquippedObject::GetSightOrigin() const
{
	return RelativeSightOrigin;
}




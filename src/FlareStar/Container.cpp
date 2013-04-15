#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\container.h"
#include "Room.h"
#include "CommonDeclarations.h"
#include "Utilities.h"
#include "GenericWeapon.h"
#include "RoomsManager.h"
#include "Roomable.h"
#include "ObjectsLibrary.h"
#include "BonusesKeeper.h"
#include "Bonus.h"

Container::Container(void) : 
SceneObject(),
Trigger()
{
}

Container::~Container(void)
{
}

bool Container::Init()
{
	Trigger::Init();
    SceneObject::Init();	

    return true;
}

void Container::Step( unsigned timeMs )
{
	assert(Node);
	if (NULL==Node)
		return;

	TriggersPosition = Node->getPosition();

	Trigger::Step(timeMs);
	SceneObject::Step(timeMs);	   
}

bool Container::ApplyIn(IAAObject* obj)
{		
	Trigger::ApplyIn(obj);
	int whole=0, applyed=0;
	const char *str;
	bool RegisterDropableWeapon;
	
	RegisterDropableWeapon = false;
		TiXmlElement *xml_element = 0;
		xml_element = Content->FirstChildElement("contents");
		if (xml_element) 
		{				
			xml_element = xml_element->FirstChildElement("content");
			while(xml_element)
			{	
				++whole;
				//str = xml_element->Attribute("class");

				int res, val;
				str = xml_element->Attribute("class");
				assert(str);
				if (strcmp(str,"weapon")==0)
				{
					List<IEquipped::SBonus>::ListNode *bonus=NULL, *begin=NULL;
					IEquipped *eq = obj->GetEquipped();

					if (eq)
					{
						bonus=NULL;
						begin=eq->GetBonuses()->GetBegin();
						for (bonus = begin;bonus!=NULL && bonus->Value.Inited;bonus=bonus->Next) {;}

						//List<IEquipped::SBonus>::ListNode *bonus = eq->GetBonuses()->GetBegin();
						//if (!bonus || !bonus->Value.Inited) //eq->GetBonuses()->GetBegin()->IsEmpty()
						if (!begin || bonus) // если нет бонусов вообще или есть непроинициализированные
						{								
							res = xml_element->QueryIntAttribute("lid", &val);
							assert(TIXML_SUCCESS==res);
							if (TIXML_SUCCESS==res)
							{
								Weapon *content = ObjectsLibrary::GetInstance()->LoadWeapon(val);

								if (eq->AddEquipment(content))
								{						
									content->SetParent(obj->GetScenable());
									content->Init();							
									++applyed;
								} else
									delete content;
							}
						}
						if (NULL == bonus)
							RegisterDropableWeapon = true;
					}										
				} else
					if (strcmp(str,"bonus")==0)
					{
						BonusesKeeper *bk = obj->GetBonusesKeeper();
						if (bk)
						{
							res = xml_element->QueryIntAttribute("type", &val);
							if (bk->TestSlot(val))
							{
								res = xml_element->QueryIntAttribute("lid", &val);
								assert(TIXML_SUCCESS==res);
								if (TIXML_SUCCESS==res)
								{
									Bonus *bonus= ObjectsLibrary::GetInstance()->LoadBonus(val);
									if (bk->AddBonus(bonus))
									{	
										++applyed;
									} else
										delete bonus;
								}
							}	
						}
					}

				xml_element = xml_element->NextSiblingElement();
			}
		}	 
		//assert(whole==applyed);
		if (applyed)
		{
			if (RegisterDropableWeapon)
			{
				IEquipped::SBonus bonus;
				bonus.LID = LID;
                bonus.DescriptableID = DescriptableID;
				bonus.Inited=true;
				bonus.DropProbability=0;
				IEquipped *eq = obj->GetEquipped();
				if (eq)
					eq->AddBonus(bonus);
			}			

			CommonDeclarations::DeleteObjectRequest(this);
			return true;
		}
	
	
	return false;
}

bool Container::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened


	const char *str;    

	str = xml->Value();

	/*cfg = xml->Attribute("cfg");

	if (cfg)
	{
		return XMLConfigurable::Parse(cfg);
	}*/

	Trigger::Parse(xml);
	SceneObject::Parse(xml);

	if (strcmp(str,"trigger")==0) 
	{	
		Content.reset(xml->Clone());        
	}

	return true;
}

Ogre::AxisAlignedBox Container::GetBoundingBox(bool transformed) const
{		
	return Trigger::GetBoundingBox(transformed);
}


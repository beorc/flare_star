#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "AIModule.h"
#include "Room.h"
//#include "Core.h"
#include "CommonDeclarations.h"

#include "aicommander.h"
#include "AIStrategy.h"

//#include "aigonshootattackstartegy.h"
#include "AIRayPathFinderStrategy.h"
#include "AIBasicPathFinderStrategy.h"
#include "AIRowFormationCommander.h"
#include "AIAckerFormationCommander.h"
#include "AILookStrategy.h"
#include "AISightNAttackStrategy.h"
#include "AIInConeAttackStrategy.h"
#include "AIFlyNAttack1Strategy.h"
#include "AIAutoPilotStrategy.h"

// todo
/*
-сделать анимацию для ии-объектов
*/

AIModule::AIModule(void): 
CommanderModule(NULL),
PathFinder(NULL),
UnitType(UT_NONE),
LeadModule(NULL),
AttackMode(IAI::AM_NONE),
AutoPilot(NULL),
Active(true)
{
}

AIModule::~AIModule(void)
{
	//if (LeadModule)
	//	LeadModule->RemoveSlave(Owner);
    //StartegiesPool::iterator iPos=Strategies.begin(), iEnd=Strategies.end();
	//
	//if (CommanderModule)
	//{
	//	if (CommanderModule->PassToSlave())
	//	{
	//		//Strategies.Remove(CommanderModule);
 //           return;
	//	}
	//}
	for (StartegiesPool::ListNode *pos = Strategies.GetBegin();pos!=NULL;pos=pos->Next)
    {
		delete pos->Value;
    }
	Strategies.Clear();
}

void AIModule::StepAI(unsigned timeMs)
{	
	if (!Active)
		return;
    //StartegiesPool::iterator iPos=Strategies.begin(), iEnd=Strategies.end();
    for (StartegiesPool::ListNode *pos = Strategies.GetBegin();pos!=NULL;pos=pos->Next)
    {
        pos->Value->Step(timeMs);
    }

    //LastTime = timeMs;	
}

AICommander *AIModule::GetCommander()
{
    return CommanderModule;
}

void AIModule::SetLead(AICommander *lead)
{
	LeadModule = lead;
}

void AIModule::SetUnitType(EUnitType type)
{
    UnitType = type;
}

AIModule::EUnitType AIModule::GetUnitType()
{
    return UnitType;
}

void AIModule::AddStrategy(AIStrategy *strategy)
{
    /*
    StartegiesPool::iterator iPos=Strategies.begin(), iEnd=Strategies.end();
    for (;iPos!=iEnd;++iPos)
    {
    if ((*iPos)->GetType()==strategy->GetType())
    {
    delete (*iPos);
    Strategies.erase(iPos);
    break;
    }
    }
    */	
    if (strategy->GetType()==AIStrategy::ST_COMMANDER) 
    {
        if (CommanderModule)
        {
            EraseStrategy((AIStrategy *)CommanderModule);		
        }
        CommanderModule = (AICommander *)strategy;
        UnitType = UT_MASTER;
		if (LeadModule)
			LeadModule->RemoveSlave(Owner);
    }
    if (strategy->GetType()==AIStrategy::ST_PATHFINDER) 
    {
        if (PathFinder)
        {
            EraseStrategy((AIStrategy *)PathFinder);		
        }
        PathFinder = (AIPathFinderStrategy *)strategy;
    }
	if (strategy->GetType()==AIStrategy::ST_AUTOPILOT) 
	{
		if (AutoPilot)
		{
			EraseStrategy((AIStrategy *)AutoPilot);		
		}
		AutoPilot = (AIAutoPilotStrategy *)strategy;
	}
    Strategies.PushBack(strategy);    
    strategy->SetParent(Owner);
	strategy->SetOwner(this);
}

void AIModule::EraseStrategy(AIStrategy *strategy)
{	
    //StartegiesPool::iterator iPos=Strategies.begin(), iEnd=Strategies.end();
    
	for (StartegiesPool::ListNode *pos = Strategies.GetBegin();pos!=NULL;pos=pos->Next)
    {
        if (pos->Value==strategy)
        {
			Strategies.Remove(pos);
            delete strategy;            
        }
    }				
}

AIPathFinderStrategy *AIModule::GetPathFinder()
{
    return PathFinder;
}

bool AIModule::Parse(TiXmlElement* xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    const char *str;	

    str = xml->Value();
    assert(strcmp(str,"AI")==0);
	
	str = xml->Attribute("attack_mode");
	if (str)
	{
		if (strcmp(str,"aliens")==0)
		{
			AttackMode = IAI::AM_ALIENS;
		} else
		if (strcmp(str,"friends")==0)
		{
			AttackMode = IAI::AM_FRIENDS;
		}
	}

    TiXmlElement *xml_element = 0;
    xml_element = xml->FirstChildElement("strategies");
    if (xml_element) 
    {				
        xml_element = xml_element->FirstChildElement("strategy");
        while (xml_element)
        {	
            AIStrategy *strategy=NULL;
            const char *classname = xml_element->Attribute("class");		
            if (strcmp("AIRayPathFinderStrategy",classname)==0) 
            {
                strategy = new AIRayPathFinderStrategy();					
            } else
                if (strcmp("AIRowFormationCommander",classname)==0) 
                {
                    strategy = new AIRowFormationCommander;														
                } else
                    if (strcmp("AILookStrategy",classname)==0) 
                    {
                        strategy = new AILookStrategy();											
					} else
						if (strcmp("AISightNAttackStrategy",classname)==0) 
						{
							strategy = new AISightNAttackStrategy();											
                        } else
                            if (strcmp("AIFlyNAttack1Strategy",classname)==0) 
                            {
                                strategy = new AIFlyNAttack1Strategy();											
                            } else
                                if (strcmp("AIBasicPathFinderStrategy",classname)==0) 
                                {
                                    strategy = new AIBasicPathFinderStrategy();											
								} else
									if (strcmp("AIAckerFormationCommander",classname)==0) 
									{
										strategy = new AIAckerFormationCommander();											
									} else
										if (strcmp("AIInConeAttackStrategy",classname)==0) 
										{
											strategy = new AIInConeAttackStrategy();											
										} else
											if (strcmp("AIAutoPilotStrategy",classname)==0) 
											{
												strategy = new AIAutoPilotStrategy();											
											}										
                                            										
                    assert(strategy);
                    AddStrategy(strategy);
                    strategy->Parse(xml_element);

                    xml_element = xml_element->NextSiblingElement("strategy");
        }			
    }

    //AIEngine::GetInstance()->RegisterObject(this);


    return false;
}

IAAObject *AIModule::GetOwner()
{
    return Owner;
}

void AIModule::SetOwner(IAAObject *owner)
{
    Owner = owner;	
	for (StartegiesPool::ListNode *pos = Strategies.GetBegin();pos!=NULL;pos=pos->Next)
	{
		pos->Value->SetParent(Owner);
		pos->Value->Init();
	}
}

bool AIModule::Init()
{	
	for (StartegiesPool::ListNode *pos = Strategies.GetBegin();pos!=NULL;pos=pos->Next)
	{
		pos->Value->Init();
	}
	return true;
}

int AIModule::SelectTargetID()
{
	if(AM_NONE==AttackMode)
		return 0;
	/*CommonDeclarations::IDObjectsPool *objects = CommonDeclarations::GetObjects();
	CommonDeclarations::IDObjectsPool::iterator iPos = objects->begin(), iEnd = objects->end();
	IAAObject *obj=NULL;
	for (;iPos!=iEnd;++iPos)
	{
		obj = iPos->second;
		if (AttackMode==AM_ALIENS && obj->GetGroup()==IAAObject::FA_ALIEN)
			return obj->GetScriptable()->GetID();
		if (AttackMode==AM_FRIENDS && obj->GetGroup()==IAAObject::FA_FRIEND)
			return obj->GetScriptable()->GetID();
	}*/
	CommonDeclarations::ObjectsPool *objects = CommonDeclarations::GetObjects();
	
	IAAObject *obj=NULL;
	for (CommonDeclarations::ObjectsPool::ListNode *pos = objects->GetBegin();pos!=NULL;pos=pos->Next)
	{
		obj = pos->Value;
		IScriptable *scr = obj->GetScriptable();
		if (scr)
		{
			if (AttackMode==AM_ALIENS && obj->GetGroup()==IAAObject::FA_ALIEN)
				return scr->GetID();
			if (AttackMode==AM_FRIENDS && obj->GetGroup()==IAAObject::FA_FRIEND)
				return scr->GetID();
		}		
	}
	return 0;
}

bool AIModule::TryToDestroy()
{
    if (LeadModule)
        LeadModule->RemoveSlave(Owner);
    
    if (CommanderModule)
    {
        if (CommanderModule->PassToSlave())
        {            
            return false;
        }
    }
    return true;
}

AIAutoPilotStrategy *AIModule::GetAutoPilotStrategy() const
{
	return AutoPilot;
}

 bool AIModule::GetActive() const
{
	return Active;
}

void AIModule::SetActive(bool on)
{
	Active = on;
}




#pragma once

#include "List.h"

class IAAObject;
class Bonus;

class BonusesKeeper
{
public:
	struct SBonusSlot {
		enum {UNIVERSAL_SLOT=777};
		int		Type;		
		Bonus*	ChildBonus;
	};
	struct SBonusDescription
	{
		int LID;
		bool Inited;
		int Active, ActivateAfterDrop;
		char DropProbability;
	};
	typedef List<SBonusSlot> SlotsPool;
	typedef List<SBonusDescription> BonusesContainer;
		
	BonusesKeeper(void);	
	virtual ~BonusesKeeper(void);

	virtual bool AddBonus(Bonus* bonus);
	virtual bool RemoveBonus(Bonus* bonus);

	virtual void Step( unsigned timeMs );
	virtual bool Parse(TiXmlElement* xml);

	void SetOwner(IAAObject *owner);
	IAAObject *GetOwner();
	SlotsPool *GetSlots();

	//void Init();

	bool TestSlot(int type);

	virtual void Drop(bool prob_mode=true);
	virtual void Clear();
protected:
	SlotsPool		Slots;
    IAAObject     *Owner;
	BonusesContainer Bonuses;
};

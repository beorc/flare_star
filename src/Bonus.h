#pragma once

class BonusesKeeper;
class IAAObject;

class Bonus
{
public:
	struct SModifiers
	{
		SModifiers();
		bool Parse(TiXmlElement*);
		float SpeedMult;
	};
	Bonus(void);
	virtual ~Bonus(void);

	int GetSlotType();
	
	virtual bool Parse(TiXmlElement* xml);
	virtual void SetOwner(BonusesKeeper *);
	bool ApplyIn(IAAObject*);
	virtual void Step( unsigned timeMs );
	bool IsStorable() const;
	int GetDescriptableID() const;

protected:	
	BonusesKeeper *Owner;
	int SlotType;
	bool Storable;
	SModifiers Modifiers;
	int Timeout, ActiveTime;
	int DescriptableID;
};

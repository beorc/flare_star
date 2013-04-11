#pragma once
#include "idestructable.h"
#include "XMLConfigurable.h"

class Destructable : public IDestructable, public XMLConfigurable
{
public:
	Destructable(void);
	~Destructable(void);

	virtual void Damage(int damage);
	virtual int GetHealth() const;
	void SetHealth(int health);
	virtual int GetMaxHealth() const;

	virtual int GetLifesNumber() const;
	void SetLifesNumber(int num);

	virtual bool Parse(TiXmlElement* xml);

	virtual void SaveTo(TiXmlElement * root) const;
protected:
	int Health, MaxHealth, LifesNumber;
};

#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\destructable.h"

Destructable::Destructable(void) :
Health(0),
MaxHealth(0),
LifesNumber(1)
{
}

Destructable::~Destructable(void)
{
}

void Destructable::Damage(int damage)
{
	Health-=damage;
}

int Destructable::GetHealth() const
{
	return Health;
}

int Destructable::GetMaxHealth() const
{
	return MaxHealth;
}

bool Destructable::Parse(TiXmlElement* xml)
{
	if (xml == 0)
		return false; // file could not be opened

	const char *str;

	str = xml->Value();

	str = xml->Attribute("cfg");

	if (str)
	{
		return XMLConfigurable::Parse(str);
	}

	int res;
	res = xml->QueryIntAttribute("max_health",&MaxHealth);
	assert(TIXML_SUCCESS==res);
	res = xml->QueryIntAttribute("health",&Health);
	assert(TIXML_SUCCESS==res);

	return true;
}

void Destructable::SaveTo(TiXmlElement * root) const
{	
	root->SetAttribute("health", Health);
}

void Destructable::SetHealth(int health)
{
	Health = health;
}

int Destructable::GetLifesNumber() const
{
	return LifesNumber;
}

void Destructable::SetLifesNumber(int num)
{
	LifesNumber = num;
}



#pragma once

#include "ISerializable.h"

class IDestructable : public ISerializable
{
public:    
	virtual void Damage(int damage) = 0;
	virtual int GetHealth() const = 0;
	virtual void SetHealth(int health) = 0;
	virtual int GetMaxHealth() const = 0;
	
	virtual int GetLifesNumber() const = 0;
	virtual void SetLifesNumber(int num) = 0;

};

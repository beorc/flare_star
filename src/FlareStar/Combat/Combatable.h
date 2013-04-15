#pragma once

#include "XMLConfigurable.h"
#include "ICombatable.h"

class Combatable : public ICombatable
{
public:
	Combatable();
	virtual ~Combatable(void);

	virtual void Step(unsigned timeMs);

protected:
};

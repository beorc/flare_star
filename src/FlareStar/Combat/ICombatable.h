#pragma once

class ICombatable
{
public:
	virtual void Step(unsigned timeMs) = 0;
};

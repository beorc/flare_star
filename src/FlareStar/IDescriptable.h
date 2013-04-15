#pragma once

class TiXmlElement;

class IDescriptable
{
public:	
	virtual char *GetIconName() const = 0;
	virtual char *GetHeader() const = 0;
	virtual char *GetText() const = 0;	
};



#pragma once

class TiXmlElement;

class ISerializable
{
public:
	virtual void SaveTo(TiXmlElement * root) const = 0;
};

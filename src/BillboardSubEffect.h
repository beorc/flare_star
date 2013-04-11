#pragma once
#include "BaseSubEffect.h"
#include "CommonDeclarations.h"
//#include "XMLConfigurable.h"
#include "HashMap.h"

namespace Ogre
{
    class BillboardSet;
}

class BillboardSubEffect : public BaseSubEffect //, public XMLConfigurable
{
public:	
	OBJDESTROY_DEFINITION
	
	typedef AAHashMap<Ogre::BillboardSet*> BillboardSetsContainer;
    BillboardSubEffect(void);
    virtual ~BillboardSubEffect(void);

    virtual bool Parse(TiXmlElement* xml);
	virtual bool Init();
	virtual void Step(unsigned timeMs);
	
	static void Reset();
	static void Close();
		
protected:
    char *MaterialName;
	bool PointRendering;
	Ogre::BillboardSet *BillborardSetReference;
	Ogre::Billboard *OgreBillboard;
	int Size;

	static BillboardSetsContainer *GetBillboardSets();
};

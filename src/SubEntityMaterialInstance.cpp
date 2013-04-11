#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\subentitymaterialinstance.h"

SubEntityMaterialInstance::SubEntityMaterialInstance (Ogre::SubEntity *se) : MaterialInstance () {
    mSubEntity = se;

    initOriginalMaterial ();
}

SubEntityMaterialInstance::~SubEntityMaterialInstance () {
    // Reset to the original material
	if (!mOriginalMat.isNull())
		mSubEntity->setMaterialName (mOriginalMat->getName ());
}

void SubEntityMaterialInstance::setMaterialName (Ogre::String name) {
    clearCopyMaterial ();

    mSubEntity->setMaterialName (name);

    initOriginalMaterial ();

    setTransparency (mCurrentTransparency);
}

void SubEntityMaterialInstance::setTransparency (Ogre::Real transparency) {
    if (!MaterialInstance::setTransparency (transparency))
        return;
	
    if (mCurrentTransparency > 0.0f) {		
		Ogre::String matname = mCopyMat->getName();
		if (mSubEntity->getMaterialName()!=matname)
			mSubEntity->setMaterialName(matname);
    }
    else {
		if (!mOriginalMat.isNull())
		{
			Ogre::String matname = mOriginalMat->getName();
			if (mSubEntity->getMaterialName()!=matname)
				mSubEntity->setMaterialName (mOriginalMat->getName());
		} else
			mOriginalMat=mOriginalMat;
		
    }
}

void SubEntityMaterialInstance::initOriginalMaterial () {
    mOriginalMat = Ogre::MaterialManager::getSingleton ().getByName(mSubEntity->getMaterialName());
}

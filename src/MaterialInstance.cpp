#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\materialinstance.h"

MaterialInstance::MaterialInstance () {
    mCurrentTransparency = 0.0f;

    mCopyMat.setNull ();

    mSBT = Ogre::SBT_TRANSPARENT_ALPHA;
}

MaterialInstance::~MaterialInstance () {
    clearCopyMaterial ();
}

void MaterialInstance::setSceneBlending (Ogre::SceneBlendType sbt) {
    mSBT = sbt;

    if (!mCopyMat.isNull ()) {
        Ogre::Material::TechniqueIterator techniqueIt = mCopyMat->getTechniqueIterator ();
        while (techniqueIt.hasMoreElements ()) {
            Ogre::Technique *t = techniqueIt.getNext ();
            Ogre::Technique::PassIterator passIt = t->getPassIterator ();
            while (passIt.hasMoreElements ()) {
                passIt.getNext ()->setSceneBlending (mSBT);
            }
        }
    }
}

bool MaterialInstance::setTransparency (Ogre::Real transparency) {
    mCurrentTransparency = transparency;
    if (mCurrentTransparency > 0.0f) {
        if (mCurrentTransparency > 1.0f)
            mCurrentTransparency = 1.0f;

        if (mCopyMat.isNull ()) {
            createCopyMaterial ();
            if (mCopyMat.isNull())
                return false;
        }

        unsigned short i = 0, j;
        Ogre::ColourValue sc, dc; // Source colur, destination colour
        Ogre::Material::TechniqueIterator techniqueIt = mCopyMat->getTechniqueIterator ();
        while (techniqueIt.hasMoreElements ()) {
            Ogre::Technique *t = techniqueIt.getNext ();
            Ogre::Technique::PassIterator passIt = t->getPassIterator ();
            j = 0;
            while (passIt.hasMoreElements ()) {
                sc = mOriginalMat->getTechnique (i)->getPass (j)->getDiffuse ();

                switch (mSBT) {
          case Ogre::SBT_ADD:
              dc = sc;
              dc.r -= sc.r * mCurrentTransparency;
              dc.g -= sc.g * mCurrentTransparency;
              dc.b -= sc.b * mCurrentTransparency;
              passIt.peekNext ()->setAmbient (Ogre::ColourValue::Black);
              break;
          case Ogre::SBT_TRANSPARENT_ALPHA:
          default:
              dc = sc;
              dc.a = sc.a * (1.0f - mCurrentTransparency);
              passIt.peekNext ()->setAmbient (mOriginalMat->getTechnique (i)->getPass (j)->getAmbient ());
              break;
                }
                passIt.peekNext ()->setDiffuse (dc);
                passIt.moveNext ();

                ++j;
            }

            ++i;
        }
    }

    return true;
}

Ogre::MaterialPtr MaterialInstance::getCopyMaterial () {
    return mCopyMat;
}

void MaterialInstance::createCopyMaterial () {
    Ogre::String name;
    if (mOriginalMat.isNull())
        return;
    // Avoid name collision
    do {        
        name = mOriginalMat->getName () + Ogre::StringConverter::toString (Ogre::Math::UnitRandom ());
    } while (Ogre::MaterialManager::getSingleton ().resourceExists (name));

    mCopyMat = mOriginalMat->clone (name);

    Ogre::Material::TechniqueIterator techniqueIt = mCopyMat->getTechniqueIterator ();
    while (techniqueIt.hasMoreElements ()) {
        Ogre::Technique *t = techniqueIt.getNext ();
        Ogre::Technique::PassIterator passIt = t->getPassIterator ();
        while (passIt.hasMoreElements ()) {
            passIt.peekNext ()->setDepthWriteEnabled (false);
            passIt.peekNext ()->setSceneBlending (mSBT);
            passIt.moveNext ();
        }
    }
}

void MaterialInstance::clearCopyMaterial () {
    if (!mCopyMat.isNull ())
        Ogre::MaterialManager::getSingleton ().remove (mCopyMat->getName ());

    mCopyMat.setNull ();
}

#pragma once
#include "Ogre.h"

class MaterialInstance
{
public:
    MaterialInstance(void);
    virtual ~MaterialInstance(void);

    void setSceneBlending (Ogre::SceneBlendType sbt);
    /** Changes this instance transparency. 
    * @param transparency The new transparency. Values will be clamped to [0..1].
    * @note This changes transparency. A value of 0 means full opacity, while 1 means full 
    *       transparency (invisible)
    * @note If transparency equals 0, it will use the original material instead of the copy 
    *       (the copy is mandatory transparent, and thus might be slower than the original).
    */
    bool setTransparency (Ogre::Real transparency);
    /** Retrieves a shared pointer to its cloned material.
    * @return A MaterialPtr of the cloned material.
    */
    Ogre::MaterialPtr getCopyMaterial ();

protected:
    /** Reference to the original material.
    */
    Ogre::MaterialPtr mOriginalMat;
    /** Reference to the copy material.
    */
    Ogre::MaterialPtr mCopyMat;
    /** Keeps the current transparency value.
    */
    Ogre::Real mCurrentTransparency;
    /** Current blending method.
    */
    Ogre::SceneBlendType mSBT;
    // Methods ====================================================================================

    /** Initialises the reference to the original material.
    */
    virtual void initOriginalMaterial () = 0;
    /** Clones the original material.
    */
    void createCopyMaterial ();
    /** If exists, removes the copy material, and clears the reference to it.
    */
    void clearCopyMaterial ();
};

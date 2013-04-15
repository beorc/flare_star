#pragma once
#include "SubEntityMaterialInstance.h"
#include "Ogre.h"
#include <vector>

/** Iterator to traverse the SubEntityMaterialInstance's.
* @author Kencho.
*/
typedef Ogre::VectorIterator<std::vector<SubEntityMaterialInstance *> > SubEntityMaterialInstancesIterator;

class EntityMaterialInstance
{
public:
    /** Constructor. 
    * Initialises the list of SubEntities' material instances.
    * @param e The entity this material instance will affect to.
    * @note This will create material instances for all the underlying SubEntities.
    */
    EntityMaterialInstance (Ogre::Entity *e);
    /** Destructor.
    * Destroys all the underlying SubEntityMaterialInstances.
    */
    ~EntityMaterialInstance ();
    /** Assigns this material to all the SubEntities through their respective 
    * SubEntityMaterialInstances.
    * @param name Name of the new material for this entity (all of its SubEntities).
    * @see SubEntityMaterialInstance::setMaterialName().
    */

    void setMaterialName (Ogre::String name);
    /** Sets the scene blending method for all the SubEntities.
    * @param sbt The desired SceneBlendType.
    * @see SubEntityMaterialInstance::setSceneBlending().
    */
    void setSceneBlending (Ogre::SceneBlendType sbt);
    /** Changes the whole Entity transparency, through all the underlying SubEntityMaterialInstances.
    * @param transparency New transparency.
    * @see SubEntityMaterialInstance::setTransparency().
    */
    void setTransparency (Ogre::Real transparency);
    /** Returns an iterator to traverse all the underlying MaterialInstances.
    * @return The SubEntityMaterialInstances iterator.
    */
    SubEntityMaterialInstancesIterator getSubEntityMaterialInstancesIterator ();

	Ogre::Real getTransparency();

protected:
    /** List of SubEntities' material instances.
    */
    std::vector<SubEntityMaterialInstance *> mSEMIs;
    /** Keeps the current transparency value.
    * @see SubEntityMaterialInstance::mCurrentTransparency.
    */
    Ogre::Real mCurrentTransparency;
};

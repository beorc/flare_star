#pragma once
#include <Ogre.h>

#include "CommonDeclarations.h"

#include "XMLConfigurable.h"
#include "ICollidable.h"
#include "List.h"

class CollisionModel3D;
class CollisionDetection;
class IAAObject;

//#define SHOW_COLLID_DATA

class Collidable : public XMLConfigurable, public ICollidable
{
public:
    //enum EType {PT_STATIC, PT_DYNAMIC};

    Collidable(void);
    virtual ~Collidable(void);

    void    InitCollisionModel();

    virtual bool	Collide(ICollidable* object);	
    virtual bool	Collide(const Ogre::Ray &ray);
    virtual bool	Collide(const Ogre::Vector3 &origin, const float &radius);

   

    virtual void AddCollision(ICollidable *obj);
    void ClearCollisions();
    bool IsInCollisions(ICollidable *obj);
    bool HaveCollisions() const;
    void UpdateCollisionModelTranformation();
    void GetCollisionModelTranformation(float m[]);
    CollisionModel3D * GetCollisionModel();
    virtual bool Parse(TiXmlElement* xml);
    CollisionDetection *GetCollisionDetection() const;
    void SetCollisionDetection(CollisionDetection *module);

	bool GetCollising() const;
    
    //virtual bool IsCollided();	
    //EType GetType() const {return Type;}

	void SetOwner(IAAObject *owner);
	const IAAObject *GetOwner() const;

	virtual void SaveTo(TiXmlElement * root) const;

	char *GetCollisionModelname() const;

	static void getMeshInformation( const Ogre::Mesh* const mesh, size_t &vertex_count,
        Ogre::Vector3* &vertices,
        size_t &index_count, unsigned long* &indices,
        const Ogre::Vector3 &position,
        const Ogre::Quaternion &orient,
        const Ogre::Vector3 &scale );

protected:    

    //EType Type;	
    
    //physics optimization
    
    List<ICollidable*> Collisions;		
	std::auto_ptr<CollisionDetection> CollisionDetectionModule;
    CollisionModel3D *CollisionModel;    
    char *CollisionModelname;

	//bool Collising;
	IAAObject *Owner;
	bool Collising;
#ifdef SHOW_COLLID_DATA
	Ogre::SceneNode *DebugNode;
	Ogre::MovableObject *DebugOgreEntity;
#endif
};

#pragma once
#include "AAObject.h"
#include "Ogre.h"
#include "OgreAL.h"
#include "IScenable.h"

#include "Array.h"
#include "CommonDeclarations.h"
//#include "List.h"

class EntityMaterialInstance;
class IMasterEffect;
class Collidable;
class Roomable;
class CollisionResponse;

class AIModule;
class EquippedObject;
class Physical;
class Destructable;
class Descriptable;
class Combatable;

class SceneObject : public AAObject, public IScenable
{
public:    		
	OBJDESTROY_DEFINITION
	//typedef Array<Effect *> EffectsPool;
	typedef List<IMasterEffect *> EffectsPool;
    //typedef std::vector<OgreAL::Sound *> SoundsPool;
    //typedef Array<OgreAL::Sound *> SoundsPool;
    SceneObject();
    
    virtual ~SceneObject(void);
	
    Ogre::SceneNode *GetNode() const;
    
    Ogre::MovableObject *GetEntity() {return OgreEntity;}
	void SetEntity(Ogre::MovableObject *);
    void SetTransparency(Ogre::Real percent);
    void CreateEntityMaterial();
    void RemoveEntityMaterial();
    EntityMaterialInstance *GetEntityMaterial();
	Ogre::Real GetTransparencyPercent();
	virtual bool Parse(TiXmlElement* xml);
	virtual void Step(unsigned timeMs);
	virtual bool Killed();
	
	
	virtual Ogre::Quaternion GetOrientation();
	virtual void SetOrientation(Ogre::Quaternion direction);

	virtual Ogre::Quaternion GetInitialRotation() const;
	void SetModel(const char* modelname);
	virtual bool	Init();
	virtual bool InitEntity();
	virtual bool InitNode(Ogre::SceneNode *parent);
    //virtual bool InitSounds();
	virtual void SetParent(IScenable *parent);
	
	virtual Ogre::AxisAlignedBox GetBoundingBox(bool transformed=false) const;
	virtual void UpdateBox();

	virtual void SetPosition(Ogre::Vector3 pos);	
    virtual Ogre::Vector3 GetPosition() const;
	virtual Ogre::Vector3 GetInitialPosition() const;
	    
    EObjType GetType() const {return Type;}
    void SetType(EObjType type) {Type=type;}
    virtual void GetBoundingPoints(float Side, Ogre::Vector3 outPoints[]);
    
    virtual void Register();
    virtual void Unregister();

    void UpdatePosition();

    void SetInitialPosition(Ogre::Vector3 pos);
	void SetInitialDirection(Ogre::Vector3 dir);
	
    virtual void SetDirection(Ogre::Vector3 direction);
    virtual Ogre::Vector3 GetScale() const;
    virtual Ogre::Quaternion GetAbsoluteOrientation() const;
    void	SetNode(Ogre::SceneNode *node);
    virtual void SetAiModule(AIModule *module);
    virtual AIModule *GetAiModule();

    virtual IPhysical *GetPhysical() const;
    virtual IRoomable *GetRoomable() const;
    virtual IScenable *GetScenable() const;
	virtual ICollidable *GetCollidable() const;
    virtual IAI *GetAI() const;
	virtual IEquipped *GetEquipped() const;
	virtual IDestructable *GetDestructable() const;
	//virtual IDescriptable *GetDescriptable() const;
	virtual ICombatable *GetCombatable() const;
	virtual int GetDescriptableID() const;
	
	virtual void SaveTo(TiXmlElement * root) const;

	virtual int GetTag() const;
	virtual void SetTag(int tag);

    virtual Ogre::Vector3 GetPrevPosition() const;
	
	virtual bool AddEffect(int lid, bool need_init);
	virtual bool RemoveEffect(int lid);
	virtual bool RemoveEffect(const char* name);

	virtual bool RestoreBackupPosition();

    virtual void SetAI(IAI *);
	virtual BonusesKeeper *GetBonusesKeeper();

	const char *GetModelName() const;
    	
protected:
	Ogre::SceneNode *Node;
    Ogre::MovableObject *OgreEntity;
 
    EntityMaterialInstance *EntityMaterial;
	EffectsPool *Effects;
    //std::auto_ptr<SoundsPool> Sounds;
	Ogre::Quaternion InitialRotation, InvertedInitialOrientation;	
	char *Modelname;
	bool SceneObjectInited;

	//std::auto_ptr< List<char*> > SoundFiles;
	std::auto_ptr<Collidable> CollidableModule;
    std::auto_ptr<Roomable> RoomableModule;
    //std::auto_ptr<AIModule> AiStaff;
    AIModule    *AiStaff;
    std::auto_ptr<EquippedObject> EquippedStaff;
    std::auto_ptr<Physical> PhysicalModule;
	std::auto_ptr<Destructable> DestructableModule;
	std::auto_ptr<BonusesKeeper> BonusesModule;
	//IDescriptable *DescriptableReference;
	std::auto_ptr<Combatable> CombatableUnit;
	int DescriptableID;
	
    EObjType Type;	
    
    Ogre::Vector3 InitialPosition, InitialDirection;
    //unsigned LastUpdateTime;
    //unsigned LastTimes[2];
    //bool RunEffectsOnCreate;
	int Irx, Iry, Irz;
	Ogre::AxisAlignedBox BoundingBoxCache, Box;
	bool HaveCachedAABB;
    Ogre::Vector3 PrevPosition;	
};

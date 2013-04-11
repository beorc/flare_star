#pragma once
#include "Equipment.h"

class IMasterEffect;
class IAmmo;
//class AmmoFactory;

class Weapon : public Equipment
{
public:
	
	struct WeaponDescription {
		WeaponDescription();
		int ShootingTimeout, FirstShootTimout;
		int KickBuff;
		int DamageBuff, SpeedBuff;
		float Dispersion;
	};
	enum EShootingMode {SM_NOTSHOOTING, SM_SHOOTING, SM_TRYING_TO_SHOOT};
	Weapon();
	//Weapon(const char* modelname);
	//Weapon(Ogre::SceneNode* node);
	virtual ~Weapon(void);

	virtual void Step( unsigned timeMs );
	//void SetAmmoCreator(AmmoFactory *creator);
	void SetShootingMode(EShootingMode mode);

	virtual bool Parse(TiXmlElement* xml);

	virtual bool Init();

	void SetAmmoLID(char ammo_lid);

	virtual void SetTargetPosition(Ogre::Vector3 pos);
	virtual void ApplyWeapon(IAmmo *);

	virtual int GetAmmoVelocity();
	virtual void UpdateAmmoVelocity();
	virtual bool TryToShoot(Ogre::Vector3 tpos, int tradius);
	virtual void UpdateBox();
protected:
	virtual void Shoot();
	
	unsigned LastShootTime;
	EShootingMode ShootimgMode;
	//AmmoFactory *AmmoCreator;
	int AmmoLID, AmmoVelocity;
	IMasterEffect *ShootEffect;
    Ogre::Quaternion RelOrientation;
    float BarrelLength;

	WeaponDescription Description;

	Ogre::Vector3 TargetPosition;
	Ogre::Vector3 CrosshairRelPosition;
	char *CrosshairMaterial;
	int MinCrosshairDistance, MaxCrosshairDistance, Size;
	Ogre::BillboardSet *CrosshairBillboards;
	Ogre::SceneNode *CrosshairNode;

    unsigned MaxCharge, CurrentCharge;
	float AngleCosinusModule;
	/*
	int ShootingTimeout, FirstShootTimout;
	int KickBuff;
	int DamageBuff, SpeedBuff;
	float Dispersion;*/
};

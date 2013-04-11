#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "GenericWeapon.h"
#include "GenericAmmo.h"
//#include "AmmoFactory.h"
#include "ObjectsLibrary.h"

GenericWeapon::GenericWeapon() :
Weapon()
{
}

GenericWeapon::~GenericWeapon(void)
{
}

void GenericWeapon::Shoot()
{	
	//char log[100];
	
	Weapon::Shoot();

	//GenericAmmo *ammo = (GenericAmmo*)ObjectsLibrary::GetInstance()->Load(AmmoLID);
	IAmmo *ammo = ObjectsLibrary::GetInstance()->LoadAmmo(AmmoLID);
	
    Vector3 RelPosition(0,0,BarrelLength);
    RelPosition = Node->getWorldOrientation()*RelOrientation*RelPosition;
	Vector3 pos = Node->getWorldPosition()+RelPosition;
	Quaternion rot = Node->getWorldOrientation()*RelOrientation;		
	if (CrosshairNode)
	{
		if (!TargetPosition.isZeroLength())
		{
			Ogre::Vector3 p1=Ogre::Vector3::NEGATIVE_UNIT_Z, p2=TargetPosition - pos;
			Ogre::Quaternion q;
			p1 = rot * p1;
			q = p1.getRotationTo(p2);
			rot = q * rot;
		}
	}
	
	ammo->Init(pos,rot);	
	ammo->ApplyWeapon(Description);
	ApplyWeapon(ammo);
}

void GenericWeapon::SaveTo(TiXmlElement * root) const
{
	TiXmlElement *elem = new TiXmlElement( "weapon" );
	root->LinkEndChild( elem );  
	
	elem->SetAttribute("class", "GenericWeapon");

	if (CfgReference)
		elem->SetAttribute("cfg", CfgReference);
}


#include "StdAfx.h"
//#include <tinyxml.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\weapon.h"
//#include "AmmoFactory.h"
#include "IMasterEffect.h"
#include "CommonDeclarations.h"
#include "AACamera.h"
#include "IAmmo.h"
#include "ObjectsLibrary.h"

static const int MinCrosshairDist = 1500, SqMinCrosshairDist=MinCrosshairDist*MinCrosshairDist;

Weapon::WeaponDescription::WeaponDescription () :
ShootingTimeout(0), 
FirstShootTimout(0),
KickBuff(0),
DamageBuff(0), 
SpeedBuff(0),
Dispersion(0.f)
{
}

Weapon::Weapon() :
Equipment(ET_WEAPON),
ShootimgMode(SM_NOTSHOOTING),
ShootEffect(NULL),
RelOrientation(Ogre::Quaternion::IDENTITY),
BarrelLength(0),
AmmoLID(0),
TargetPosition(Ogre::Vector3::ZERO),
CrosshairMaterial(NULL),
MinCrosshairDistance(-1),
MaxCrosshairDistance(-1),
CrosshairNode(NULL),
CrosshairBillboards(NULL),
MaxCharge(0), 
CurrentCharge(0),
AmmoVelocity(10000),
AngleCosinusModule(1)
{
}

Weapon::~Weapon(void)
{
	if (CrosshairMaterial)
		delete [] CrosshairMaterial;
}

void Weapon::SetAmmoLID(char ammo_lid)
{
	AmmoLID  = ammo_lid;
	UpdateAmmoVelocity();
}

void Weapon::UpdateAmmoVelocity()
{
	IAmmo *ammo = ObjectsLibrary::GetInstance()->LoadAmmo(AmmoLID);
	AmmoVelocity = abs(ammo->GetVelocity());
	ammo->Destroy();
}

void Weapon::SetShootingMode(Weapon::EShootingMode mode)
{
	ShootimgMode=mode;
}

void Weapon::Step( unsigned timeMs )
{		
	SceneObject::Step(timeMs);
	LastShootTime += timeMs;

	if (CrosshairNode)
	{		
		if (!TargetPosition.isZeroLength() && (TargetPosition-Parent->GetPosition()).squaredLength()>SqMinCrosshairDist)
		{	
			Ogre::Camera *cam = CommonDeclarations::GetCamera()->GetOgreCamera();
			Ogre::Vector3 campos = cam->getWorldPosition(), newtarget;
			Ogre::Vector3 dir = TargetPosition - campos;
			dir.normalise();
			newtarget = campos + dir*MaxCrosshairDistance;

			Ogre::Vector3 relpos = newtarget-Node->getWorldPosition();
			relpos = Node->getWorldOrientation().Inverse()*relpos;


			//relpos = cam->getWorldOrientation().Inverse()*relpos;

			/*Ogre::Vector3 dir = CrosshairRelPosition;			
			dir.x = -Node->getPosition().z;
			CrosshairNode->setPosition(dir);*/

			CrosshairNode->setPosition(relpos);                            			
			
		} else
		{
			CrosshairNode->setPosition(CrosshairRelPosition);			
		}
	}	

    //int diff = timeMs-LastShootTime;
	if (SM_SHOOTING == ShootimgMode && LastShootTime>=(unsigned)Description.ShootingTimeout) 
	{
		//char log[100];
		//sprintf(log,"%d %d\n",LastShootTime,timeMs);
		//Debugging::Log("wa",log);
		LastShootTime = 0;
		Shoot();
	}        
}

bool Weapon::Parse(TiXmlElement* xml)
{
	const char *str;	
	str = xml->Attribute("cfg");
	if (str)
	{
		return XMLConfigurable::Parse(str);
	}
	
	Equipment::Parse(xml);

	if (xml == 0)
		return false; // file could not be opened
	
	str = xml->Value();
	assert(strcmp(str,"weapon")==0);
	if (strcmp(str,"weapon")==0) 
	{			
		int res = xml->QueryIntAttribute("timeout", &Description.ShootingTimeout);
		assert(TIXML_SUCCESS==res);
		
		Description.Dispersion = 0.f;
		xml->QueryFloatAttribute("dispersion", &Description.Dispersion);
		//assert(TIXML_SUCCESS==res);
		//if (TIXML_SUCCESS==res)
		/*{
			Ogre::Radians angle(Description.Dispersion);
			Description.Dispersion = Ogre::Math::Cos(angle.valueRadians());
		}*/

        float dirx, diry, dirz;
        res = xml->QueryFloatAttribute("dirx", &dirx);
        assert(TIXML_SUCCESS==res);
        res = xml->QueryFloatAttribute("diry", &diry);
        assert(TIXML_SUCCESS==res);
        res = xml->QueryFloatAttribute("dirz", &dirz);
        assert(TIXML_SUCCESS==res);

        Ogre::Vector3 relative_dir(dirx,diry,dirz);
        RelOrientation = relative_dir.getRotationTo(Ogre::Vector3::UNIT_Z);

        res = xml->QueryFloatAttribute("barrel", &BarrelLength);
        assert(TIXML_SUCCESS==res);

		TiXmlElement *xml_element = 0;
		xml_element = xml->FirstChildElement("ammo");

		if (xml_element)
		{	
			res = xml_element->QueryIntAttribute("lid",&AmmoLID);
			assert(TIXML_SUCCESS==res);
			if (TIXML_SUCCESS == res)
				UpdateAmmoVelocity();
		}			

		xml_element = xml->FirstChildElement("crosshair");
		if (xml_element)
		{	
			str = xml_element->Attribute("material");
			assert(NULL!=str);
			CrosshairMaterial = AAUtilities::StringCopy(str);

			res = xml_element->QueryIntAttribute("min_distance",&MinCrosshairDistance);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryIntAttribute("max_distance",&MaxCrosshairDistance);
			assert(TIXML_SUCCESS==res);

			res = xml_element->QueryIntAttribute("size",&Size);
			assert(TIXML_SUCCESS==res);
		}

		int val;
		res = xml->QueryIntAttribute("cone_angle", &val);
		//assert(TIXML_SUCCESS==res);
		if (TIXML_SUCCESS==res)
		{
			Ogre::Degree angle(val);		
			AngleCosinusModule = Ogre::Math::Cos(angle.valueRadians());
		}
	}		
	return true;
}

bool Weapon::Init()
{   
	bool res = Equipment::Init();
	//EffectsPool::iterator iPos = Effects->begin(), iEnd = Effects->end();
	if (Effects)
	{
		for (EffectsPool::ListNode *pos = Effects->GetBegin();pos!=NULL;pos = pos->Next)
		{
			if (pos->Value->GetType()==IMasterEffect::MET_ONSHOOT) {
				ShootEffect = pos->Value;
			}
		}
	}
	
	if (Parent->GetTag()==T_PLAYER && MinCrosshairDistance>=0)
	{
		assert(CrosshairMaterial);
		if (CrosshairMaterial)
		{
			char *buffer = CommonDeclarations::GenGUID();

			Ogre::SceneManager *scene_manager = CommonDeclarations::GetSceneManager();
			CrosshairBillboards = scene_manager->createBillboardSet(buffer,1);

			delete [] buffer;

			CrosshairBillboards->setMaterialName(CrosshairMaterial);
			CrosshairBillboards->setDefaultDimensions(Size,Size);

			CrosshairBillboards->createBillboard(Ogre::Vector3::ZERO);
			
			CrosshairRelPosition = Ogre::Vector3(0,0,-MaxCrosshairDistance);
			CrosshairRelPosition = RelOrientation*CrosshairRelPosition;
			//Vector3 pos = Node->getWorldPosition()+RelPosition;

			CrosshairNode = Node->createChildSceneNode();			 //Ogre::Vector3(0,0,-MaxCrosshairDistance)
			CrosshairNode->setPosition(CrosshairRelPosition);

			//CrosshairBillboards->getMaterial()->setDepthCheckEnabled(false);
			//CrosshairBillboards->getMaterial()->setDepthWriteEnabled(false);
			//CrosshairBillboards->getMaterial()->setFog(true);
			CrosshairBillboards->setRenderQueueGroup(RENDER_QUEUE_6);
			CrosshairNode->attachObject(CrosshairBillboards);			
		}
	}
	return res;
}

void Weapon::Shoot()
{
	if (ShootEffect)
		ShootEffect->Start();
}

void Weapon::SetTargetPosition(Ogre::Vector3 pos)
{
	TargetPosition = pos;
}

void Weapon::ApplyWeapon(IAmmo *ammo)
{
	if (Owner) 
	{
		ammo->SetAmmoGroup(Owner->GetOwner()->GetGroup());
	}
}

int Weapon::GetAmmoVelocity()
{
	return AmmoVelocity;
}

bool Weapon::TryToShoot(Ogre::Vector3 tpos, int tradius)
{
	//Vector3 RelPosition(0,0,BarrelLength);
	//RelPosition = Node->getWorldOrientation()*RelOrientation*RelPosition;
	//Vector3 pos = Node->getWorldPosition()+RelPosition;
	Vector3 pos = Node->getWorldPosition();
	Quaternion rot = Node->getWorldOrientation()*RelOrientation;

	Ogre::Vector3 src = tpos - pos, direction = rot*Ogre::Vector3::NEGATIVE_UNIT_Z;
	src.normalise();

	Ogre::Real cs = src.dotProduct(direction);
	if (src.isZeroLength() || cs>=AngleCosinusModule)
	{				
		return true;
	} else
	{	
		return false;
	}
	
	return false;
}

void Weapon::UpdateBox()
{	
}



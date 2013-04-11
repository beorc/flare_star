#include "StdAfx.h"
#include <squirrel.h>
#include <sqstdio.h>
#include <sqstdaux.h>

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "Core.h"
#include ".\scriptmanager.h"
#include "AACamera.h"
#include "AApplication.h"
#include "Debugging.h"
#include "CommonDeclarations.h"
#include "IAAObject.h"
#include "GUISystem.h"
#include "GUIRadarLayout.h"
#include "ObjectsLibrary.h"
#include "ComixFrameListener.h"
#include "ComixFrameHandler.h"
#include "ActivationTrigger.h"
#include "Player.h"
#include "AIAutoPilotStrategy.h"
#include "Combat/CombatFrameHandler.h"
#include "Combat/CombatFrameListener.h"

//ScriptManager *ScriptManager::Instance = NULL;

void SPrint(HSQUIRRELVM v,const SQChar *s,...) 
{
	va_list arglist; 
	va_start(arglist, s); 

	char log[200];
	vsprintf(log, (const char*)s, arglist);	
	Debugging::Log("script.log",log);

	va_end(arglist); 
}

void CompileErrorHandler(HSQUIRRELVM v, const SQChar * desc,const SQChar *
						source, SQInteger line, SQInteger column)
{
	assert(false);
}

int error_handler(HSQUIRRELVM v)
{
	assert(false);
	return 0;
}

SQInteger SSGetPivotPos(HSQUIRRELVM v) // int id
{   	
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii1;
    SQRESULT res = sq_getinteger(v, 2, &ii1);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
	if (obj)
	{
		Ogre::Vector3 pos = obj->GetScenable()->GetPosition();
        sq_newarray(v,0);
        sq_pushinteger(v,(int)(pos.x));
        sq_arrayappend(v,-2);
        sq_pushinteger(v,(int)(pos.y));
        sq_arrayappend(v,-2);
        sq_pushinteger(v,(int)(pos.z));
        sq_arrayappend(v,-2);		
		return 1;
	}	
	return 0;	    
}

SQInteger SSIsCollided(HSQUIRRELVM v) // int id
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii1;
    SQRESULT res = sq_getinteger(v, 2, &ii1);

    IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
    if (obj)
    {
        bool res = obj->GetPhysical()->IsCollided();        
        sq_pushbool(v,(SQBool)(res));        
        return 1;
    }

    return 0;
}

SQInteger SSSwitchCamera(HSQUIRRELVM v) // int id
{
        SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii[6];
	SQInteger mode;
	SQRESULT res = sq_getinteger(v, 2, &mode);
	assert(SQ_SUCCEEDED(res));
	switch (mode)
	{
		case 0:
		{
			Ogre::Camera *cam = CommonDeclarations::GetCamera()->GetOgreCamera();
			if (cam)
			{	
				CommonDeclarations::GetApplication()->SetCurrentCamera( cam );
			}
			break;
		}
		case 1:
		{
			res = sq_getinteger(v, 3, &ii[0]);
			res = sq_getinteger(v, 4, &ii[1]);
			res = sq_getinteger(v, 5, &ii[2]);
			res = sq_getinteger(v, 6, &ii[3]);
			res = sq_getinteger(v, 7, &ii[4]);
			res = sq_getinteger(v, 8, &ii[5]);
			//res = sq_getinteger(v, 9, &ii[6]);
			
			Ogre::Vector3 pos(ii[0],ii[1],ii[2]), dir(ii[3],ii[4],ii[5]);
			Ogre::Camera *cam = CommonDeclarations::GetApplication()->GetScriptsCamera();
			if (cam)
			{	
				cam->setPosition(pos);
				cam->setDirection(dir);
				CommonDeclarations::GetApplication()->SetCurrentCamera(cam);		
			}
			break;
		}
		case 2:
		{
			res = sq_getinteger(v, 3, &ii[0]);
			res = sq_getinteger(v, 4, &ii[1]);
			res = sq_getinteger(v, 5, &ii[2]);
			res = sq_getinteger(v, 6, &ii[3]);
			res = sq_getinteger(v, 7, &ii[4]);
			res = sq_getinteger(v, 8, &ii[5]);
			//res = sq_getinteger(v, 9, &ii[6]);
			
			Ogre::Vector3 pos(ii[0],ii[1],ii[2]), dir(ii[3],ii[4],ii[5]);
			Ogre::Camera *cam = CommonDeclarations::GetApplication()->GetCombatCamera();
			if (cam)
			{	
				cam->setPosition(pos);
				cam->setDirection(dir);
				CommonDeclarations::GetApplication()->SetCurrentCamera(cam);		
			}
			break;
		}
	};
	/*
	if (bb[0])
	{
		res = sq_getinteger(v, 3, &ii[0]);
		res = sq_getinteger(v, 4, &ii[1]);
		res = sq_getinteger(v, 5, &ii[2]);
		res = sq_getinteger(v, 6, &ii[3]);
		res = sq_getinteger(v, 7, &ii[4]);
		res = sq_getinteger(v, 8, &ii[5]);
		//res = sq_getinteger(v, 9, &ii[6]);
		
		Ogre::Vector3 pos(ii[0],ii[1],ii[2]), dir(ii[3],ii[4],ii[5]);
		Ogre::Camera *cam = CommonDeclarations::GetApplication()->GetScriptsCamera();
		if (cam)
		{	
			cam->setPosition(pos);
			cam->setDirection(dir);
			CommonDeclarations::GetApplication()->SetCurrentCamera(cam);		
		}
	} else
	{
		Ogre::Camera *cam = CommonDeclarations::GetCamera()->GetOgreCamera();
		if (cam)
		{	
			CommonDeclarations::GetApplication()->SetCurrentCamera( cam );
		}
	}
	*/

    return 0;
}

SQInteger SSSetFog(HSQUIRRELVM v) // int id
{
    SQInteger nargs = sq_gettop(v); //number of arguments	
	assert(nargs==9);
    SQInteger ii[5];
	SQFloat ff[3];
    SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);
	res = sq_getinteger(v, 6, &ii[4]);
	
	res = sq_getfloat(v, 7, &ff[0]);
	res = sq_getfloat(v, 8, &ff[1]);
	res = sq_getfloat(v, 9, &ff[2]);
	
	Ogre::FogMode mode = (Ogre::FogMode)ii[1];

	Ogre::Real r(ii[2]/255.f), g(ii[3]/255.f), b(ii[4]/255.f);
	const Ogre::ColourValue colour(r,g,b);

	
	//if (ii[0]==0)
		CommonDeclarations::GetSceneManager()->setFog(mode, colour, ff[0], ff[1], ff[2]);
	//else
	//{
	//	/*Ogre::SceneNode *skybox = CommonDeclarations::GetSceneManager()->getSkyBoxNode();
	//	if (skybox)
	//		skybox->getMaterial()->setFog(true, mode, colour, ff[0], ff[1], ff[2]);*/

	//	MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("Sky");
	//	mat->setFog(true, mode, colour, ff[0], ff[1], ff[2]);
	//}
		

    return 0;
}

SQInteger SSActivateObject(HSQUIRRELVM v) // int id
{
    SQInteger nargs = sq_gettop(v); //number of arguments	
	//assert(nargs==7);
	SQInteger ii[1];
    SQBool bb[1];
    
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);    	
	res = sq_getbool(v, 3, &bb[0]);
	
	/*IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
    if (obj)
    {
		obj->SetActive(bb[0] ? true : false);
    }	*/
	CommonDeclarations::ActivateObject(ii[0], bb[0] ? true : false);

    return 0;
}

SQInteger SSBlockPlayer(HSQUIRRELVM v) // int id
{
    SQInteger nargs = sq_gettop(v); //number of arguments	
	//assert(nargs==7);	
    SQBool bb[1];
    
	SQRESULT res;
	res = sq_getbool(v, 2, &bb[0]);	
	
	CommonDeclarations::BlockPlayer(bb[0] ? true : false);    

    return 0;
}

SQInteger SSSetVisible(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments	
	//assert(nargs==7);
	SQInteger ii[1];
	SQBool bb[1];

	SQRESULT res = sq_getinteger(v, 2, &ii[0]);    	
	res = sq_getbool(v, 3, &bb[0]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		IScenable *scen = obj->GetScenable();
		scen->GetNode()->setVisible(bb[0] ? true : false);
	}

	return 0;
}

SQInteger SSSetCollided(HSQUIRRELVM v) // int id
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii1;
    SQBool bb1;
    SQRESULT res = sq_getinteger(v, 2, &ii1);
    res = sq_getbool(v, 3, &bb1);

    IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
    if (obj)
    {
		obj->GetPhysical()->SetCollided(bb1 ? true : false);
    }

    return 0;
}

SQInteger SSSetThrottle(HSQUIRRELVM v)
{
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii[4];    
    SQRESULT res = sq_getinteger(v, 2, &ii[0]);
        
	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

    IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
    if (obj)
    {
        obj->GetPhysical()->SetThrottle(Ogre::Vector3(ii[1],ii[2],ii[3]));
    }

    return 0;
}

SQInteger SSSetVelocityVector(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];    
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetVelocityVector(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}

	return 0;
}

SQInteger SSSetLinVelocity(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];    
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetLinVelocity(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}

	return 0;
}

SQInteger SSSetLinVelocityAccel(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];    
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetLinVelocityAccel(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}

	return 0;
}

SQInteger SSGetLinVelocity(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii1;
	SQRESULT res = sq_getinteger(v, 2, &ii1);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
	if (obj)
	{
		Ogre::Vector3 vect = obj->GetPhysical()->GetLinVelocity();
		sq_newarray(v,0);
		sq_pushinteger(v,(int)(vect.x));
		sq_arrayappend(v,-2);
		sq_pushinteger(v,(int)(vect.y));
		sq_arrayappend(v,-2);
		sq_pushinteger(v,(int)(vect.z));
		sq_arrayappend(v,-2);		
		return 1;
	}
    return 0;
}

SQInteger SSGetRotVelocity(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii1;
	SQRESULT res = sq_getinteger(v, 2, &ii1);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
	if (obj)
	{
		Ogre::Vector3 vect = obj->GetPhysical()->GetRotVelocity();
		sq_newarray(v,0);
		sq_pushinteger(v,(int)(vect.x));
		sq_arrayappend(v,-2);
		sq_pushinteger(v,(int)(vect.y));
		sq_arrayappend(v,-2);
		sq_pushinteger(v,(int)(vect.z));
		sq_arrayappend(v,-2);		
		return 1;
	}
    return 0;
}

SQInteger SSSetDirection(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	
	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetScenable()->SetDirection(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}
    return 0;
}

SQInteger SSSetAcceleration(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	
	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetAcceleration(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}
    return 0;
}

SQInteger SSSetForwardDirection(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	
	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	res = sq_getinteger(v, 5, &ii[3]);

	IAAObject *obj = (IAAObject*)CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetForwardDirection(Ogre::Vector3(ii[1],ii[2],ii[3]));
	}
    return 0;
}

SQInteger SSSetScriptValue(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getinteger(v, 3, &ii[1]);
	res = sq_getinteger(v, 4, &ii[2]);
	
	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetScriptable()->SetScriptValue(ii[1],(char)ii[2]);
	}
	return 0;
}

SQInteger SSGetScriptValue(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[2];
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	res = sq_getinteger(v, 3, &ii[1]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		char val = obj->GetScriptable()->GetScriptValue(ii[1]);
		sq_pushinteger(v,(int)val);		
		return 1;
	}
	return 0;
}

SQInteger SSSetLinDamping(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];
	SQFloat ff;
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getfloat(v, 3, &ff);
	
	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetLinDamping(ff);
	}
	return 0;
}

SQInteger SSSetRotDamping(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[4];
	SQFloat ff;
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getfloat(v, 3, &ff);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		obj->GetPhysical()->SetRotDamping(ff);
	}
	return 0;
}

SQInteger SSAddForce(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[7];	
	SQFloat ff[7];
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	res = sq_getfloat(v, 3, &ff[1]);
	res = sq_getfloat(v, 4, &ff[2]);
	res = sq_getfloat(v, 5, &ff[3]);

	res = sq_getfloat(v, 6, &ff[4]);
	res = sq_getfloat(v, 7, &ff[5]);
	res = sq_getfloat(v, 8, &ff[6]);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		Ogre::Vector3 pos = obj->GetScenable()->GetPosition();
		obj->GetPhysical()->AddForce(pos+Ogre::Vector3(ff[1],ff[2],ff[3]), Ogre::Vector3(ff[4],ff[5],ff[6]));
	}
	return 0;
}

SQInteger SSDestroy(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii;	
	SQRESULT res = sq_getinteger(v, 2, &ii);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii);
	if (obj)
	{	
		CommonDeclarations::DeleteObjectRequest(obj);
	}
	return 0;
}

SQInteger SSRand(HSQUIRRELVM v)
{	
	//sq_pushfloat(v,rand()/(RAND_MAX+1.0));	
	sq_pushfloat(v,AAUtilities::Rand());	
	return 1;	
}

SQInteger SSGetRadius(HSQUIRRELVM v)
{	
	SQInteger ii;	
	SQRESULT res = sq_getinteger(v, 2, &ii);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii);
	if (obj)
	{
		sq_pushinteger(v,obj->GetPhysical()->GetRadius());		
		return 1;
	}	
	return 0;	
}

SQInteger SSGetHealth(HSQUIRRELVM v)
{	
	SQInteger ii;	
	SQRESULT res = sq_getinteger(v, 2, &ii);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii);
	if (obj)
	{
		IDestructable *destr = obj->GetDestructable();
		if (destr)
			sq_pushinteger(v,destr->GetHealth());
		else
			sq_pushinteger(v,-333);
		return 1;
	} else
		sq_pushinteger(v,-333);
	return 1;	
}

SQInteger SSGetMass(HSQUIRRELVM v)
{	
	SQInteger ii;	
	SQRESULT res = sq_getinteger(v, 2, &ii);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii);
	if (obj)
	{
		sq_pushinteger(v,obj->GetPhysical()->GetMass());		
		return 1;
	}	
	return 0;	
}

SQInteger SSSetScriptsCameraTrack(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[1];
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);	
	SQBool bb[1];
	res = sq_getbool(v, 3, &bb[0]);

	Ogre::Camera *cam = CommonDeclarations::GetApplication()->GetScriptsCamera();
	if (cam)
	{
		if (bb[0])
		{
			IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
			if (obj)
			{
				cam->setAutoTracking(true, obj->GetScenable()->GetNode());			
			}
		} else
		{
			cam->setAutoTracking(false);
		}
	}

	return 0;
}

SQInteger SSProcessEffect(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[2];
	SQBool bb[1];
	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]); // id of object
	res = sq_getinteger(v, 3, &ii[1]); // lid of effect
	//const SQChar str[50];
	//res = sq_getstring(v, 3, &str);

	res = sq_getbool(v, 4, &bb[0]); // target state
		
	if (ii[0]==-1)
	{
		if (bb[0])
		{
			CommonDeclarations::GetCamera()->SetFogMirror(ii[1]);
		} else
		{
			CommonDeclarations::GetCamera()->RemoveFogMirror();
		}		
	} else
	{
		IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
		if (obj)
		{
			if (bb[0])
			{
				obj->GetScenable()->AddEffect(ii[1], true);
			} else
			{
				obj->GetScenable()->RemoveEffect(ii[1]);
			}
		}
	}	
	

	return 0;
}

SQInteger SSSetScriptHandler(HSQUIRRELVM v) // int id
{	
	SQInteger nargs = sq_gettop(v); //number of arguments	
	SQRESULT res;	
	SQInteger ii[2];
	res = sq_getinteger(v, 2, &ii[0]);

	const SQChar *str = NULL;	
	res = sq_getstring(v, 3, &str);

	res = sq_getinteger(v, 4, &ii[1]);

	
	CommonDeclarations::SetScriptHandler(ii[0], (char*)str, ii[1]);
	
	return 0;
}

SQInteger SSSetHudVisible(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments	
	SQRESULT res;	
	SQBool bb[1];
	res = sq_getbool(v, 2, &bb[0]);
	
	GUISystem::GetInstance()->SetHudVisible(bb[0] ? true : false);
	
	//CommonDeclarations::SetScriptHandlersActive(bb[0] ? true : false);
	
	return 0;
}

SQInteger SSSetCompasTarget(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments
		
	switch(nargs) {
	case 3:
		{
			SQInteger ii[2];
			SQRESULT res = sq_getinteger(v, 2, &ii[0]); // id of object
			assert(SQ_SUCCEEDED(res));
			res = sq_getinteger(v, 3, &ii[1]); // lid of effect
			assert(SQ_SUCCEEDED(res));
			
			GUIRadarLayout *radar = (GUIRadarLayout *)GUISystem::GetInstance()->GetRadar();
			if (ii[0])
			{		
				radar->SetCompasTarget(ii[0], ii[1]);
			} else
				radar->RemoveCompasTarget();
		}
		break;
	case 4:
		{
			SQInteger ii[3];
			SQRESULT res = sq_getinteger(v, 2, &ii[0]); // id of object
			assert(SQ_SUCCEEDED(res));
			res = sq_getinteger(v, 3, &ii[1]);
			assert(SQ_SUCCEEDED(res));
			res = sq_getinteger(v, 4, &ii[2]);			
			assert(SQ_SUCCEEDED(res));

			GUIRadarLayout *radar = (GUIRadarLayout *)GUISystem::GetInstance()->GetRadar();
					
			Ogre::Vector3 pos(ii[0], ii[1], ii[2]);
			radar->SetCompasTarget(pos);
		}
		break;	
	};
	


	return 0;
}

SQInteger SSSetMusicGain(HSQUIRRELVM v) // int id
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQFloat ff[1];

	SQRESULT res = sq_getfloat(v, 2, &ff[0]); // id of object
	

	CommonDeclarations::SetMusicGain(ff[0]);

	return 0;
}

SQInteger SSGetMusicGain(HSQUIRRELVM v)
{		
	SQFloat f = (SQFloat)CommonDeclarations::GetMusicGain();
	sq_pushfloat(v,f);
		
	return 1;
}

SQInteger SSGetDirection(HSQUIRRELVM v) // int id
{   	
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii1;
	SQRESULT res = sq_getinteger(v, 2, &ii1);

	IAAObject *obj = CommonDeclarations::GetIDObject(ii1);
	if (obj)
	{
		Ogre::Quaternion orient = obj->GetScenable()->GetOrientation();
		Ogre::Vector3 dir = Ogre::Vector3::NEGATIVE_UNIT_Z;
		dir = orient * dir;
		
		sq_newarray(v,0);
		sq_pushfloat(v,dir.x);
		sq_arrayappend(v,-2);
		sq_pushfloat(v,dir.y);
		sq_arrayappend(v,-2);
		sq_pushfloat(v,dir.z);
		sq_arrayappend(v,-2);		
		return 1;
	}	
	return 0;	    
}

SQInteger SSSetGameState(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[1];

	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	CommonDeclarations::GetApplication()->SetCurrentGameState(ii[0]);
	
	return 0;
}

SQInteger SSSetHandler(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	SQInteger ii[2];

	SQRESULT res = sq_getinteger(v, 2, &ii[0]);

	switch(ii[0]) {
		case 0:
		{			
			break;
		}		
		case 1:
		{
            res = sq_getinteger(v, 3, &ii[1]);
			ComixFrameHandler *current_comix = new ComixFrameHandler();
			TiXmlElement *xml = ObjectsLibrary::GetInstance()->Get(ii[1]);
			current_comix->Parse(xml);
			ComixFrameListener *listener = ComixFrameListener::GetInstance();
			listener->SetCurrentHandler(current_comix);
			break;	
		}		
		case 2:
		{
            res = sq_getinteger(v, 3, &ii[1]);
			CombatFrameHandler *handler = new CombatFrameHandler();
			TiXmlElement *xml = ObjectsLibrary::GetInstance()->Get(ii[1]);
			if (xml)
				handler->Parse(xml);
			handler->Init();
			handler->InitObjects();
			CombatFrameListener *listener = CombatFrameListener::GetInstance();
			listener->SetCurrentHandler(handler);
			break;	
		}		
	};

	//CommonDeclarations::GetApplication()->SetCurrentGameState(ii[0]);

	return 0;
}

SQInteger SSCloseHandler(HSQUIRRELVM v)
{
	SQInteger nargs = sq_gettop(v); //number of arguments
	
	ComixFrameListener *listener = ComixFrameListener::GetInstance();
	listener->SetCurrentHandler(NULL);

	return 0;
}

SQInteger SSSetRoomOnly(HSQUIRRELVM v)
{	
	SQInteger ii[2];	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);	
	assert(SQ_SUCCEEDED(res));
	res = sq_getinteger(v, 3, &ii[1]);
	assert(SQ_SUCCEEDED(res));

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		IRoomable *rmbl = obj->GetRoomable();
		if (rmbl)
			rmbl->SetRoomOnly(ii[1]);
	}	
	return 0;	
}

SQInteger SSSwitchTrigger(HSQUIRRELVM v)
{	
	SQInteger ii[1];	
	SQRESULT res = sq_getinteger(v, 2, &ii[0]);	
	assert(SQ_SUCCEEDED(res));
	SQBool bb;
	res = sq_getbool(v, 3, &bb);
	assert(SQ_SUCCEEDED(res));

	
	ActivationTrigger *trigger = Core::GetInstance()->GetTriggerByID(ii[0]);
	if (trigger)
	{	
		if (bb)
			trigger->ApplyIn(NULL);
		else
			trigger->ApplyOut(NULL);
	}	
	return 0;	
}

SQInteger SSSetAutoPilotTarget(HSQUIRRELVM v)
{	
	SQInteger nargs = sq_gettop(v); //number of arguments

	Player *player = CommonDeclarations::GetPlayer();
	IAI *ai = player->GetAI();
	if (NULL==ai)
		return 0;
	
	AIAutoPilotStrategy *auto_pilot = ai->GetAutoPilotStrategy();
	if (NULL==auto_pilot)
	{
		return 0;
	}

	switch(nargs) {
	case 2:
		{
			SQInteger ii[1];
			SQRESULT res = sq_getinteger(v, 2, &ii[0]); // id of object
			assert(SQ_SUCCEEDED(res));
			
			if (ii[0])
			{		
				auto_pilot->SetTargetID(ii[0]);
			} else
				auto_pilot->RemoveTarget();
		}
		break;
	case 4:
		{
			SQInteger ii[3];
			SQRESULT res = sq_getinteger(v, 2, &ii[0]); // id of object
			assert(SQ_SUCCEEDED(res));
			res = sq_getinteger(v, 3, &ii[1]);
			assert(SQ_SUCCEEDED(res));
			res = sq_getinteger(v, 4, &ii[2]);			
			assert(SQ_SUCCEEDED(res));

			Ogre::Vector3 pos(ii[0], ii[1], ii[2]);
			auto_pilot->SetTargetPoint(pos);
		}
		break;	
	};

	return 0;
}

SQInteger SSSetPosition(HSQUIRRELVM v) // int id
{   	
    SQInteger nargs = sq_gettop(v); //number of arguments
    SQInteger ii[4];
    SQRESULT res = sq_getinteger(v, 2, &ii[0]);
	assert(SQ_SUCCEEDED(res));

	IAAObject *obj = CommonDeclarations::GetIDObject(ii[0]);
	if (obj)
	{
		res = sq_getinteger(v, 3, &ii[1]);
		assert(SQ_SUCCEEDED(res));

		res = sq_getinteger(v, 4, &ii[2]);
		assert(SQ_SUCCEEDED(res));

		res = sq_getinteger(v, 5, &ii[3]);
		assert(SQ_SUCCEEDED(res));

		Ogre::Vector3 pos(ii[1], ii[2], ii[3]);

		obj->SetPosition(pos);		
	}	
	return 0;	    
}

//
//SQInteger SSCloseCurrentComix(HSQUIRRELVM v)
//{
//	CommonDeclarations::CloseCurrentComix();
//	return 0;
//}

void register_global_func(HSQUIRRELVM v,SQFUNCTION f,const char *fname)
{	
	SQRESULT res;
    sq_pushroottable(v);
    sq_pushstring(v,(SQChar*)fname,-1);
    sq_newclosure(v,f,0); //create a new function
    res=sq_createslot(v,-3); 
    sq_pop(v,1); //pops the root table  
}

void ScriptManager::RegisterFunctions()
{
	register_global_func(VM, SSGetPivotPos, "GetPivotPos");
	register_global_func(VM, SSIsCollided, "IsCollided");
	register_global_func(VM, SSSetCollided, "SetCollided");
	register_global_func(VM, SSSetThrottle, "SetThrottle");	
	register_global_func(VM, SSGetLinVelocity, "GetLinVelocity");	
	register_global_func(VM, SSGetRotVelocity, "GetRotVelocity");
	register_global_func(VM, SSSetDirection, "SetDirection");
	register_global_func(VM, SSSetAcceleration, "SetAcceleration");
	register_global_func(VM, SSSetForwardDirection, "SetForwardDirection");	
	register_global_func(VM, SSGetScriptValue, "GetScriptValue");	
	register_global_func(VM, SSSetScriptValue, "SetScriptValue");	
	register_global_func(VM, SSSetLinDamping, "SetLinDamping");	
	register_global_func(VM, SSSetRotDamping, "SetRotDamping");
	register_global_func(VM, SSAddForce, "AddForce");
	register_global_func(VM, SSRand, "Rand");
	register_global_func(VM, SSDestroy, "Destroy");
	register_global_func(VM, SSGetRadius, "GetRadius");	
	register_global_func(VM, SSGetMass, "GetMass");		
	
	register_global_func(VM, SSSwitchCamera, "SwitchCamera");	
	register_global_func(VM, SSSetFog, "SetFog");
	register_global_func(VM, SSActivateObject, "ActivateObject");
	register_global_func(VM, SSBlockPlayer, "BlockPlayer");
	register_global_func(VM, SSSetVisible, "SetVisible");
	register_global_func(VM, SSSetScriptsCameraTrack, "SetScriptsCameraTrack");
	register_global_func(VM, SSGetHealth, "GetHealth");
	register_global_func(VM, SSProcessEffect, "ProcessEffect");
	register_global_func(VM, SSSetScriptHandler, "SetScriptHandler");
	register_global_func(VM, SSSetHudVisible, "SetHudVisible");
	register_global_func(VM, SSSetCompasTarget, "SetCompasTarget");
	register_global_func(VM, SSSetMusicGain, "SetMusicGain");
	register_global_func(VM, SSGetMusicGain, "GetMusicGain");
	register_global_func(VM, SSGetDirection, "GetDirection");

	register_global_func(VM, SSSetGameState, "SetGameState");
	register_global_func(VM, SSSetHandler, "SetHandler");
	register_global_func(VM, SSSetRoomOnly, "SetRoomOnly");
	register_global_func(VM, SSSwitchTrigger, "SwitchTrigger");
	register_global_func(VM, SSSetAutoPilotTarget, "SetAutoPilotTarget");
	register_global_func(VM, SSSetPosition, "SetPosition");	
}

ScriptManager::ScriptManager(void):
VM(NULL)
{	
    Init();
}

ScriptManager::~ScriptManager(void)
{
    Reset();
}

ScriptManager *ScriptManager::GetInstance()
{
	static ScriptManager *Instance = new ScriptManager;
    return Instance;
}

void ScriptManager::Init()
{    
    VM = sq_open(1024); // creates a VM with initial stack size 1024 

	sq_setcompilererrorhandler(VM, CompileErrorHandler);
	sq_setprintfunc(VM, SPrint);

	/*sq_pushregistrytable(VM);
	sq_pushstring(VM,(SQChar*)"error_handler",-1);
	sq_rawget(VM,-2);
	sq_seterrorhandler(VM);
	sq_pop(VM,1);
	*/

	RegisterFunctions();
		
	//sq_seterrorhandler(VM);	
}

void ScriptManager::Close()
{   
	delete GetInstance();
}

void ScriptManager::Reset()
{
	if (VM)
	{
		sq_close(VM);	
		VM = NULL;
	}
}

int ScriptManager::CompileFile(const char *filename)
{	
    Ogre::DataStreamPtr stream = Ogre::ResourceGroupManager::getSingleton().openResource(filename, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    assert(!stream.isNull());
    if (stream.isNull())
    {		
        Ogre::StringUtil::StrStreamType errorMessage;
        errorMessage << "Could not open script file: " << filename;

        OGRE_EXCEPT
            (
            Ogre::Exception::ERR_FILE_NOT_FOUND,
            errorMessage.str(), 
            "XMLConfigurable::Parse"
            );
    }

    //Get the file contents
    //String data = stream->getAsString();

    // Read the entire buffer	
    size_t Size = stream->size();

    char* pBuf = new char[Size+1];
    // Ensure read from begin of stream
    stream->seek(0);
    stream->read(pBuf, Size);
	stream.setNull();

    pBuf[Size] = '\0';

    ++Size;

	sq_pushroottable(VM);
    SQRESULT res = sq_compilebuffer(VM, (SQChar*)pBuf, (SQInteger)Size, (SQChar*)filename, SQTrue);
	assert(SQ_SUCCEEDED(res));
	
	if (SQ_SUCCEEDED(res))
	{
		sq_push(VM,-2);
		if(SQ_SUCCEEDED(sq_call(VM,1,SQFalse,SQTrue))) {
			sq_remove(VM,-1); //removes the closure			
		} else
			sq_pop(VM,1); //removes the closure		
	}		

	delete [] pBuf;
	
    return 0;
}

bool ScriptManager::Parse(TiXmlElement* xml)
{
    assert(xml);
    if (xml == 0)
        return false; // file could not be opened

    const char *str;	
    str = xml->Attribute("cfg");
    if (str)
    {
        return XMLConfigurable::Parse(str);
    }

    str = xml->Value();
    assert(strcmp(str,"scripts")==0);
    
    TiXmlElement *xml_element = 0;
    xml_element = xml->FirstChildElement("script");	
    while (xml_element)
    {		        
        str = xml_element->Attribute("file");
        CompileFile(str);	

        xml_element = xml_element->NextSiblingElement("script");
    }

    return true;
}

SQVM *ScriptManager::GetVM()
{
    return VM;
}

bool ScriptManager::Call(char *name, char *format, bool riseerror, ...)
{    
    SQRESULT res;
    int top = sq_gettop(VM); //saves the stack size before the call

    sq_pushroottable(VM); //pushes the global table
    sq_pushstring(VM,(SQChar*)name,-1);

    res = sq_get(VM,-2);
    if(SQ_SUCCEEDED(res)) 
    { //gets the field 'foo' from the global table
        sq_pushroottable(VM); //push the 'this' (in this case is the global table)

        va_list argptr;             
        int argcntr=0;

        va_start( argptr, riseerror );
        while( *format != '\0' ) {
            // string
            if( *format == 's' ) 
            {
                char* s = va_arg( argptr, char * );                
                sq_pushstring(VM,(SQChar*)s,-1);

                //printf( "Printing a string: %s\n", s );
            }
            // character
            else if( *format == 'f' ) 
            {
                float f = (char) va_arg( argptr, float );                
                sq_pushfloat(VM,f);
                //printf( "Printing a character: %c\n", c );
                break;
            }
            // integer
            else if( *format == 'i' ) 
            {
                int d = va_arg( argptr, int );
                sq_pushinteger(VM,d);
                //printf( "Printing an integer: %d\n", d );
            }          

            *format++;
            ++argcntr;
        }   
        va_end( argptr );

        res = sq_call(VM,argcntr+1,false,true); //calls the function 
        assert(SQ_SUCCEEDED(res));
        sq_pop(VM,2); //pops the roottable and the function
        return true;
    } else
    {
        if (riseerror)
            assert(false);
    }

    sq_settop(VM,top); //restores the original stack size    
    return false;
}



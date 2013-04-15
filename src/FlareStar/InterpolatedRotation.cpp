#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include "InterpolatedRotation.h"

InterpolatedRotation::InterpolatedRotation(void) :
mRotProgress(0),
mRotFactor(0),
mRotating(false),
RotationFps(100)
{
}

void InterpolatedRotation::StartRotation(Ogre::Quaternion &src, Ogre::Quaternion &dest, float fps)
{
    RotationFps = fps;

    StartRotation(src, dest);
}

inline void InterpolatedRotation::StartRotation(Ogre::Quaternion &src, Ogre::Quaternion &dest)
{    
    mRotating = true;
    mRotFactor = 1.0f / RotationFps;	
    mOrientSrc = src;
    mOrientDest = dest;           // We want dest orientation, not a relative rotation (quat)
    mRotProgress = 0;
}

void InterpolatedRotation::Step()
{
	if(mRotating)                                // Process timed rotation
	{
		mRotProgress += mRotFactor;
		if(mRotProgress>1)
		{
			mRotating = false;
		}		
	}	
}

Ogre::Quaternion InterpolatedRotation::Slerp()
{
	return Ogre::Quaternion::Slerp(mRotProgress, mOrientSrc, mOrientDest, true);
}

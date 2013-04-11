#pragma once

#include <Ogre.h>

struct InterpolatedRotation {
	InterpolatedRotation();
	Ogre::Quaternion mOrientSrc;               // Initial orientation
	Ogre::Quaternion mOrientDest;              // Destination orientation
	Ogre::Real mRotProgress;                   // How far we've interpolated
	Ogre::Real mRotFactor;                     // Interpolation step
	bool	   mRotating;
    float RotationFps;
	void StartRotation(Ogre::Quaternion &src, Ogre::Quaternion &dest, float fps);
    void StartRotation(Ogre::Quaternion &src, Ogre::Quaternion &dest);
	Ogre::Quaternion Slerp();
	void Step();
};

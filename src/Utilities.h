#pragma once

#include "List.h"
#include "MT\SFMT.h" // !!!

class TiXmlElement;

namespace AAUtilities
{
    List<char*>* ParseStrings(TiXmlElement* xml, const char* element, const char* attr);
    char *StringCopy(const char *str);
    std::string WStringToString(const std::wstring& s);

	inline int f2i (float x)
	{
		const int magic = (150<<23)|(1<<22);
		x+= *(float*)&magic;
		return *(int*)&x - magic;
	}

	inline int ceil_int (double x)
	{
		assert (x > static_cast<double> (INT_MIN / 2) - 1.0);
		assert (x < static_cast<double> (INT_MAX / 2) + 1.0);
		const float    round_towards_p_i = -0.5f;
		int            i;
		__asm
		{
			fld   x
				fadd  st, st (0)
				fsubr round_towards_p_i
				fistp i
				sar   i,  1
		}
		return (-i);
	}

    void Norm1(Ogre::Vector3 &vect);
	void Clamp(Ogre::Vector3 &vect, double edge);

	void SRand(unsigned long s);
	
	//static float Rand();

	inline float Rand()
	{
		return gen_rand32()/(ULONG_MAX+1.f); // !!!
	}

    /*inline float Rand()
    {
        return rand()/(RAND_MAX+1.f);
    }*/
};

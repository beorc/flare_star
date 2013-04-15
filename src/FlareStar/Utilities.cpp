#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include <tinyxml.h>
#include "Utilities.h"
#include "MT\SFMT.h"

List<char*>* AAUtilities::ParseStrings(TiXmlElement* xml, const char* element, const char* attr)
{    
    //std::string tempstr;
    
    const char *str;
    char *tempstr;
    TiXmlElement* xml_element=xml;

    if (xml_element)
    {        		
        List<char *> *res = new List<char *>;

        xml_element = xml_element->FirstChildElement(element);	
        while (xml_element)
        {
            str = xml_element->Attribute(attr);

            tempstr = StringCopy(str);

            res->PushBack(tempstr);

            xml_element = xml_element->NextSiblingElement(element);
        }
        
        return res;
    }
    return NULL;
}

inline char *AAUtilities::StringCopy(const char *str)
{
    assert(str);
    if (!str)
        return NULL;
    char *str_copy = new char[strlen(str)+1];    
    strcpy(str_copy,str);
    return str_copy;
}

std::string AAUtilities::WStringToString(const std::wstring& s)
{
    std::string temp(s.length(), ' ');
    std::copy(s.begin(), s.end(), temp.begin());
    return temp; 
}

void AAUtilities::Norm1(Ogre::Vector3 &vect)
{
    float lm = std::max(abs(vect.x),abs(vect.y));
    lm = std::max(abs(lm),abs(vect.z));
    if ( lm > 1e-08 )
    {
        double fInvLength = 1.0 / lm;
        vect.x *= fInvLength;
        vect.y *= fInvLength;
        vect.z *= fInvLength;
    }    
    return;
}

void AAUtilities::Clamp(Ogre::Vector3 &vect, double edge)
{
	if (abs(vect.x)>edge) {
		vect.x=edge*abs(vect.x)/vect.x;
	}
	if (abs(vect.y)>edge) {
		vect.y=edge*abs(vect.y)/vect.y;
	}
	if (abs(vect.z)>edge) {
		vect.z=edge*abs(vect.z)/vect.z;
	}
}

void AAUtilities::SRand(unsigned long s)
{
	init_gen_rand(s);
}

//inline float AAUtilities::Rand()
//{
//	return genrand_int32()/(ULONG_MAX+1.f);	
//}


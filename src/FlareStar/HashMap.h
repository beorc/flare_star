#pragma once

#include "Array.h"

template <class T>
class AAHashMap
{
public:
    enum {EMPTY=65535};
    AAHashMap(int size);
    ~AAHashMap(void);

    bool Find(const char*, T*);
    void Insert(const char*,T);
    void Remove(const char*);
    Array<T> *GetData();
	void Clear();
protected:

    unsigned short CountCrc(const char*);

    void init_crctab();

    unsigned short crctab[65536];

    unsigned short hash[65536];

    Array<T> Data;
    size_t Count;
};



template <class T>
AAHashMap<T>::AAHashMap(int size):
Count(0)
{    
    init_crctab();
    Data.Resize(size);
    Clear();
}

template <class T>
AAHashMap<T>::~AAHashMap(void)
{
}

template <class T>
unsigned short AAHashMap<T>::CountCrc(const char* key)
{
    int i = 0;
    unsigned short crc = 0;
    do
    {
        crc = crctab[(unsigned char)key[i]] + (crc>>1);
        i++;
    } while (key[i] != 0);
    return crc;  
}

template <class T>
bool AAHashMap<T>::Find(const char* key, T* res)
{
	*res = NULL;
    int crc;
    crc = CountCrc(key);
    int n = hash[crc];
    
    if (n == EMPTY)
        return false;

    *res = Data.Data[n];

    return true;
}

template <class T>
void AAHashMap<T>::Insert(const char* key,T value)
{
    assert(Count<Data.Size);

	unsigned short crc = CountCrc(key);
	assert(hash[crc]==EMPTY);

    unsigned short i;
    for (i=0;i<Data.Size;++i)
    {
        if (Data.Data[i]==NULL)
            break;
    }
    assert(i<Data.Size);
    
    hash[crc] = i;
    Data.Data[i] = value;
    ++Count;
}

template <class T>
void AAHashMap<T>::Remove(const char* key)
{
    int crc = CountCrc(key);
    int n = hash[crc];
    assert(n != EMPTY);
	if (n!=EMPTY)
	{
		Data.Data[n] = NULL;
		hash[crc] = EMPTY;
		--Count;
	}
}

template <class T>
Array<T> *AAHashMap<T>::GetData()
{
    return &Data;
}

template <class T>
void AAHashMap<T>::init_crctab()
{
    int i, j;
    unsigned int r,cr=0;

    for (i=0; i<256; i++)
    {
        r = i;
        for (j=8; j>0; j--)
        {
            cr >>= 1;
            if (r & 1)
            {
                r = (r>>1) ^ 0xEDB88320;
                cr ^= 0xEDB88320;
            }
            else
                r >>= 1;
        }

        crctab[i] = cr;

        if (i >= 'a'  &&  i <= 'z')
            crctab[i] = crctab[i-('a'-'A')];
    }
}

template <class T>
void AAHashMap<T>::Clear()
{
	memset(hash, 255, sizeof(hash));
	memset(Data.Data, 0, sizeof(Data.Data)*Data.Size);
	Count = 0;
}


#pragma once
//#include "MemoryPool.h"

template <class T>
struct Array
{
	Array();
	~Array(void);

	void Clear();
	void Resize(size_t size);
	bool IsEmpty() const;

	T *Data;
	size_t Size;
};

template <class T>
Array<T>::Array() :
Data(NULL),
Size(0)
{
}

template <class T>
Array<T>::~Array(void)
{	
	Clear();
}

template <class T>
void Array<T>::Clear()
{
	delete [] Data;	
	
	Data = NULL;
	Size=0;
}

template <class T>
void Array<T>::Resize(size_t size)
{
	if (Data)	
	{		
		delete [] Data;
	}
    Data = new T[size];
	
	Size = size;
}

template <class T>
bool Array<T>::IsEmpty() const
{
	return 0==Size ? true : false;
}


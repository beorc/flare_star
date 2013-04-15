#pragma once

class MemAllocator
{
public:
	MemAllocator(ULONG allocation_size, ULONG count=0);
	virtual ~MemAllocator(void);

	bool Init(ULONG allocation_size, ULONG count);
	bool Init(ULONG count);
	void Clear();
	void Drop();
	PVOID Alloc();    
	void Free(PVOID ptr);    
protected:
	PBYTE Cache;
	ULONG CountFree;
	ULONG FirstFree;
	ULONG CountTotal;

	ULONG AllocationSize;

	PBYTE NodeAt(ULONG index);
	ULONG& IndexAt(ULONG index);
};

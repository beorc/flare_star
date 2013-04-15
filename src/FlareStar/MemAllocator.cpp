#include "StdAfx.h"

#ifdef MEMORY_TEST
#include "mmgr.h"
#endif

#include ".\memallocator.h"

MemAllocator::MemAllocator(ULONG allocation_size, ULONG count) :
Cache(NULL),
CountFree(0),
FirstFree(0),
CountTotal(0),
AllocationSize(allocation_size)
{
	if (count>0)
		Init(count);
}

MemAllocator::~MemAllocator(void)
{
	Clear();
}

bool MemAllocator::Init(ULONG allocation_size, ULONG count)
{
	Clear();
	AllocationSize = allocation_size;
	DWORD dwBlockSize = allocation_size + sizeof(ULONG);

	if (Cache = (PBYTE)_aligned_malloc(count * dwBlockSize, 32))
	{
		CountTotal = count;

		Drop();
		return true;

	}
	return false;
}

bool MemAllocator::Init(ULONG count) 
{ 
	return Init(AllocationSize, count); 
}

void MemAllocator::Clear()
{
	if (Cache)
	{
		_aligned_free(Cache);
		//delete [] Cache;
		Cache = NULL;
	}
	CountTotal = CountFree = FirstFree = 0;
}

void MemAllocator::Drop()
{
	for (CountFree = 0; CountFree < CountTotal; ++CountFree)
		IndexAt(CountFree) = CountFree;
}

PVOID MemAllocator::Alloc()
{
	//_aligned_malloc( AllocationSize, 32 );
	assert(CountFree);
	if (!CountFree)
	{
		return malloc(AllocationSize); //new BYTE[AllocationSize];
	}

	ULONG nFreeIndex = IndexAt(FirstFree);
	++FirstFree %= CountTotal;
	CountFree--;

	return NodeAt(nFreeIndex);
}

void MemAllocator::Free(PVOID ptr)
{
	if (CountFree < CountTotal)
	{
		ULONG nDiff = ((ULONG) ptr) - ((ULONG) Cache);
		ULONG nIndex = nDiff / (AllocationSize + sizeof(ULONG));
		if (nIndex < CountTotal)
		{
			IndexAt((FirstFree + CountFree) % CountTotal) = nIndex;
			CountFree++;
			return;
		}
	}
	free(ptr);
}

PBYTE MemAllocator::NodeAt(ULONG index)
{
	return Cache + index * (AllocationSize + sizeof(ULONG));
}

ULONG& MemAllocator::IndexAt(ULONG index)
{
	return *(ULONG*) (NodeAt(index) + AllocationSize);
}


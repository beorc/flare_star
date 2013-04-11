/******************
MemoryPool.cpp
******************/

/*!\file MemoryPool.cpp
 * \brief MemoryPool implementation.
 */
#include "StdAfx.h"
//#include "../BasicIncludes.h"
#include "SMemoryChunk.h"
#include "MemoryPool.h"
#include "..\Debugging.h"
#include "..\Utilities.h"

namespace MemPool
{
static const int FREEED_MEMORY_CONTENT        = 0xAA ; //!< Value for freed memory  
static const int NEW_ALLOCATED_MEMORY_CONTENT = 0xFF ; //!< Initial Value for new allocated memory 

bool MemoryPool::Closed=false;
SMemoryChunk *MemoryPool::m_ptrFirstChunk=NULL, *MemoryPool::m_ptrLastChunk=NULL;

/******************
Constructor
******************/
MemoryPool::MemoryPool(const std::size_t &sInitialMemoryPoolSize,
		                 const std::size_t &sMemoryChunkSize,
				         const std::size_t &sMinimalMemorySizeToAllocate,
						 bool bSetMemoryData) :
//m_ptrFirstChunk(NULL),
//m_ptrLastChunk(NULL),
m_ptrCursorChunk(NULL),
m_sTotalMemoryPoolSize(0),
m_sUsedMemoryPoolSize(0),
m_sFreeMemoryPoolSize(0),
m_sMemoryChunkSize(sMemoryChunkSize),
m_uiMemoryChunkCount(0),
m_uiObjectCount(0),
m_bSetMemoryData(bSetMemoryData),
m_sMinimalMemorySizeToAllocate(sMinimalMemorySizeToAllocate)
{
 // Allocate the Initial amount of Memory from the Operating-System...
 AllocateMemory(sInitialMemoryPoolSize) ;
}

MemoryPool *MemoryPool::GetGlobalPool()
{
	static MemoryPool *GlobalPool = new MemoryPool(DEFAULT_MEMORY_POOL_SIZE, DEFAULT_MEMORY_CHUNK_SIZE, DEFAULT_MEMORY_SIZE_TO_ALLOCATE, true);
	return GlobalPool;
}

void* MemoryPool::operator new (size_t size)
{
	return malloc(size);
}

void MemoryPool::operator delete (void* ptr)
{
	return free(ptr);
}

/******************
Destructor
******************/
MemoryPool::~MemoryPool()
{
  //Close();		
   
  // Check for possible Memory-Leaks...
  //assert((m_uiObjectCount == 0) && "WARNING : Memory-Leak : You have not freed all allocated Memory") ;
}

void MemoryPool::Close()
{	
	FreeAllAllocatedMemory() ;
	DeallocateAllChunks() ;

    delete GetGlobalPool();	
}

/******************
GetMemory
******************/
inline void *MemoryPool::GetMemory(const std::size_t &sMemorySize)
{
	static int call_count=0;
	/*char log[100];
	sprintf(log,"get %d %d (%d)\n", call_count++, sMemorySize, int(m_ptrFirstChunk->Data));
	Debugging::Log("mempool",log);
	if (call_count==53)
		call_count=call_count;*/
  std::size_t sBestMemBlockSize = CalculateBestMemoryBlockSize(sMemorySize), sNewBestMemBlockSize=0 ;  
  SMemoryChunk *ptrChunk = NULL ;
  while(!ptrChunk)
  {
    // Is a Chunks available to hold the requested amount of Memory ?
    ptrChunk = FindChunkSuitableToHoldMemory(sBestMemBlockSize) ;
	assert(ptrChunk);
    if(!ptrChunk)
    {       
	  // No chunk can be found
	  // => Memory-Pool is to small. We have to request 
	  //    more Memory from the Operating-System....      
	  sNewBestMemBlockSize = MaxValue(sBestMemBlockSize, CalculateBestMemoryBlockSize(m_sMinimalMemorySizeToAllocate)) ;
      AllocateMemory(sNewBestMemBlockSize) ;
    }
  }

  // Finally, a suitable Chunk was found.
  // Adjust the Values of the internal "TotalSize"/"UsedSize" Members and 
  // the Values of the MemoryChunk itself.
  m_sUsedMemoryPoolSize += sBestMemBlockSize ;
  assert(m_sFreeMemoryPoolSize>=sBestMemBlockSize);
  m_sFreeMemoryPoolSize -= sBestMemBlockSize ;
  m_uiObjectCount++ ;
  SetMemoryChunkValues(ptrChunk, sBestMemBlockSize) ;

  //sprintf(log,"memnew%d", call_count);
  //WriteMemoryDumpToFile(log);

  // eventually, return the Pointer to the User
  return ((void *) ptrChunk->Data) ;
}

/******************
FreeMemory
******************/
inline void MemoryPool::FreeMemory(void *ptrMemoryBlock, const std::size_t &sMemoryBlockSize)
{
	//static int call_count=0;
	//char log[100];
  // Search all Chunks for the one holding the "ptrMemoryBlock"-Pointer
  // ("SMemoryChunk->Data == ptrMemoryBlock"). Eventually, free that Chunks,
  // so it beecomes available to the Memory-Pool again...	
  SMemoryChunk *ptrChunk = FindChunkHoldingPointerTo(ptrMemoryBlock);
  FreeMemory(ptrChunk);
}

inline void MemoryPool::FreeMemory(SMemoryChunk *ptrChunk)
{
    // Search all Chunks for the one holding the "ptrMemoryBlock"-Pointer
    // ("SMemoryChunk->Data == ptrMemoryBlock"). Eventually, free that Chunks,
    // so it beecomes available to the Memory-Pool again...
    //SMemoryChunk *ptrChunk = FindChunkHoldingPointerTo(ptrMemoryBlock) ;
    if(ptrChunk)
    {	  	  
        FreeChunks(ptrChunk) ;
    }
    else
    {
        assert(false && "ERROR : Requested Pointer not in Memory Pool") ;	  
    }
    assert((m_uiObjectCount > 0) && "ERROR : Request to delete more Memory then allocated.") ;
    m_uiObjectCount-- ;
}

/******************
AllocateMemory
******************/
inline bool MemoryPool::AllocateMemory(const std::size_t &sMemorySize)
{
  // This function will allocate *at least* "sMemorySize"-Bytes from the Operating-System.

  // How it works :
  // Calculate the amount of "SMemoryChunks" needed to manage the requested MemorySize.
  // Every MemoryChunk can manage only a certain amount of Memory
  // (set by the "m_sMemoryChunkSize"-Member of the Memory-Pool).
  //
  // Also, calculate the "Best" Memory-Block size to allocate from the 
  // Operating-System, so that all allocated Memory can be assigned to a
  // Memory Chunk.
  // Example : 
  //	You want to Allocate 120 Bytes, but every "SMemoryChunk" can only handle
  //    50 Bytes ("m_sMemoryChunkSize = 50").
  //    So, "CalculateNeededChunks()" will return the Number of Chunks needed to
  //    manage 120 Bytes. Since it is not possible to divide 120 Bytes in to
  //    50 Byte Chunks, "CalculateNeededChunks()" returns 3.
  //    ==> 3 Chunks can Manage 150 Bytes of data (50 * 3 = 150), so
  //        the requested 120 Bytes will fit into this Block.
  //    "CalculateBestMemoryBlockSize()" will return the amount of memory needed
  //    to *perfectly* subdivide the allocated Memory into "m_sMemoryChunkSize" (= 50) Byte
  //    pieces. -> "CalculateBestMemoryBlockSize()" returns 150.
  //    So, 150 Bytes of memory are allocated from the Operating-System and
  //    subdivided into 3 Memory-Chunks (each holding a Pointer to 50 Bytes of the allocated memory).
  //    Since only 120 Bytes are requested, we have a Memory-Overhead of 
  //    150 - 120 = 30 Bytes. 
  //    Note, that the Memory-overhead is not a bad thing, because we can use 
  //    that memory later (it remains in the Memory-Pool).
  //

  unsigned int uiNeededChunks = CalculateNeededChunks(sMemorySize) ;
  std::size_t sBestMemBlockSize = CalculateBestMemoryBlockSize(sMemorySize) ;

  TByte *ptrNewMemBlock = (TByte *) _aligned_malloc(sBestMemBlockSize, 32) ; // allocate from Operating System
  SMemoryChunk *ptrNewChunks = (SMemoryChunk *) _aligned_malloc((uiNeededChunks * sizeof(SMemoryChunk)), 32) ; // allocate Chunk-Array to Manage the Memory
  assert(((ptrNewMemBlock) && (ptrNewChunks)) && "Error : System ran out of Memory") ;

  // Adjust internal Values (Total/Free Memory, etc.)
  m_sTotalMemoryPoolSize += sBestMemBlockSize ;
  m_sFreeMemoryPoolSize += sBestMemBlockSize ;
  m_uiMemoryChunkCount += uiNeededChunks ;

  // Usefull for Debugging : Set the Memory-Content to a defined Value
#ifdef _DEBUG
  if(m_bSetMemoryData)
  {
    memset(((void *) ptrNewMemBlock), NEW_ALLOCATED_MEMORY_CONTENT, sBestMemBlockSize) ;
  }
#endif

  // Associate the allocated Memory-Block with the Linked-List of MemoryChunks
  return LinkChunksToData(ptrNewChunks, uiNeededChunks, ptrNewMemBlock) ; ;
}

/******************
CalculateNeededChunks
******************/
inline unsigned int MemoryPool::CalculateNeededChunks(const std::size_t &sMemorySize)
{
   float f = (float) (((float)sMemorySize) / ((float)m_sMemoryChunkSize)) ;
   return ((unsigned int) AAUtilities::ceil_int(f)) ;
}

/******************
CalculateBestMemoryBlockSize
******************/
inline std::size_t MemoryPool::CalculateBestMemoryBlockSize(const std::size_t &sRequestedMemoryBlockSize)
{
  unsigned int uiNeededChunks = CalculateNeededChunks(sRequestedMemoryBlockSize) ;
  return std::size_t((uiNeededChunks * m_sMemoryChunkSize)) ;
}

/******************
FreeChunks
******************/
inline void MemoryPool::FreeChunks(SMemoryChunk *ptrChunk)
{
  // Make the Used Memory of the given Chunk available
  // to the Memory Pool again.

  SMemoryChunk *ptrCurrentChunk = ptrChunk ;
  unsigned int uiChunkCount = CalculateNeededChunks(ptrCurrentChunk->UsedSize);
  for(unsigned int i = 0; i < uiChunkCount; i++)
  {
    if(ptrCurrentChunk)
    {
      // Step 1 : Set the allocated Memory to 'FREEED_MEMORY_CONTENT'
      // Note : This is fully Optional, but usefull for debugging
	  if(m_bSetMemoryData)
	  {
        memset(((void *) ptrCurrentChunk->Data), FREEED_MEMORY_CONTENT, m_sMemoryChunkSize) ;
	  }

      // Step 2 : Set the Used-Size to Zero
      ptrCurrentChunk->UsedSize = 0 ;

      // Step 3 : Adjust Memory-Pool Values and goto next Chunk
      m_sUsedMemoryPoolSize -= m_sMemoryChunkSize ;
	  m_sFreeMemoryPoolSize += m_sMemoryChunkSize ;
      ptrCurrentChunk = ptrCurrentChunk->Next ;
	}
  }
}


/******************
FindChunkSuitableToHoldMemory
******************/
inline SMemoryChunk *MemoryPool::FindChunkSuitableToHoldMemory(const std::size_t &sMemorySize)
{
  // Find a Chunk to hold *at least* "sMemorySize" Bytes.
  unsigned int uiChunksToSkip = 0 ;
  //bool bContinueSearch = true ;
  SMemoryChunk *ptrChunk = m_ptrCursorChunk, *Selected=NULL ; // Start search at Cursor-Pos.
  size_t FreeSpace=0;
  bool EndReached=false;
  for(unsigned int i = 0; i < m_uiMemoryChunkCount; i++)
  {    
	if(ptrChunk == NULL) // End of List reached : Start over from the beginning
	{
		if (EndReached || Selected)
		{
			m_ptrCursorChunk = m_ptrLastChunk;
			return NULL;
		}
		EndReached = true;
		ptrChunk = m_ptrFirstChunk ;
	}

    if(FreeSpace + ptrChunk->DataSize >= sMemorySize)
    {
		if ((Selected && ptrChunk->IsAllocationChunk)
			|| ptrChunk->UsedSize != 0)
		{
			Selected = NULL;
			FreeSpace = 0;
		} else
		{
			FreeSpace+=m_sMemoryChunkSize;
			if (NULL==Selected)
				Selected = ptrChunk;
			if (FreeSpace >= sMemorySize) 
			{
				m_ptrCursorChunk = ptrChunk->Next ;
				return Selected ;
			}
			//m_ptrCursorChunk = ptrChunk ;
			//return ptrChunk ;
		}
    }
	if (!Selected)
	{
		uiChunksToSkip = CalculateNeededChunks(ptrChunk->UsedSize) ;
		if(uiChunksToSkip == 0) uiChunksToSkip = 1 ;
	} else
		uiChunksToSkip = 1;
    ptrChunk = SkipChunks(ptrChunk, uiChunksToSkip) ;	
  }
  m_ptrCursorChunk = m_ptrLastChunk;
  return NULL ;
}

/******************
SkipChunks
******************/
inline SMemoryChunk *MemoryPool::SkipChunks(SMemoryChunk *ptrStartChunk, unsigned int uiChunksToSkip)
{
	SMemoryChunk *ptrCurrentChunk = ptrStartChunk ;
	for(unsigned int i = 0; i < uiChunksToSkip; i++)
	{
		if(ptrCurrentChunk)
		{
		   ptrCurrentChunk = ptrCurrentChunk->Next ;
		}
		else
		{
			// Will occur, if you try to Skip more Chunks than actually available
			// from your "ptrStartChunk" 
			//assert(false && "Error : Chunk == NULL was not expected.") ;
			//break ;
			return NULL;
		}
	}
	return ptrCurrentChunk ;
}

/******************
SetMemoryChunkValues
******************/
inline void MemoryPool::SetMemoryChunkValues(SMemoryChunk *ptrChunk, const std::size_t &sMemBlockSize)
{
	assert(ptrChunk);
	SMemoryChunk *ptrCurrentChunk = ptrChunk;
	unsigned int uiChunkCount = CalculateNeededChunks(sMemBlockSize);
	for(unsigned int i = 0; i < uiChunkCount; i++)
	{
		if(ptrCurrentChunk)
		{
			ptrCurrentChunk->UsedSize = sMemBlockSize - i*m_sMemoryChunkSize;
			
			ptrCurrentChunk = ptrCurrentChunk->Next ;
		}
	}

  //if((ptrChunk)) // && (ptrChunk != m_ptrLastChunk))
  //{
  //  ptrChunk->UsedSize = sMemBlockSize ;
  //}
  //else
  //{
	 // assert(false && "Error : Invalid NULL-Pointer passed") ;
  //}
}

/******************
WriteMemoryDumpToFile
******************/
bool MemoryPool::WriteMemoryDumpToFile(const std::string &strFileName)
{
  bool bWriteSuccesfull = false ;
  std::ofstream ofOutputFile ;
  ofOutputFile.open(strFileName.c_str(), std::ofstream::out | std::ofstream::binary) ;

  SMemoryChunk *ptrCurrentChunk = m_ptrFirstChunk ;
  while(ptrCurrentChunk)
  {
    if(ofOutputFile.good())
    {
		ofOutputFile.write(((char *)ptrCurrentChunk->Data), ((std::streamsize) m_sMemoryChunkSize)) ;
      bWriteSuccesfull = true ;
    }
    ptrCurrentChunk = ptrCurrentChunk->Next ;
  }
  ofOutputFile.close() ;
  return bWriteSuccesfull ;
}

/******************
LinkChunksToData
******************/
inline bool MemoryPool::LinkChunksToData(SMemoryChunk *ptrNewChunks, unsigned int uiChunkCount, TByte *ptrNewMemBlock)
{
  SMemoryChunk *ptrNewChunk = NULL ;
  unsigned int uiMemOffSet = 0 ;
  bool bAllocationChunkAssigned = false ;
  for(unsigned int i = 0; i < uiChunkCount; i++)
  {
    if(!m_ptrFirstChunk)
    {
      m_ptrFirstChunk = SetChunkDefaults(&(ptrNewChunks[0])) ;
      m_ptrLastChunk = m_ptrFirstChunk ;
      m_ptrCursorChunk = m_ptrFirstChunk ;
    }
    else
    {
      ptrNewChunk = SetChunkDefaults(&(ptrNewChunks[i])) ;
      m_ptrLastChunk->Next = ptrNewChunk ;
      m_ptrLastChunk = ptrNewChunk ;
    }
    
	uiMemOffSet = (i * ((unsigned int) m_sMemoryChunkSize)) ;
    m_ptrLastChunk->Data = ptrNewMemBlock + uiMemOffSet;

	// The first Chunk assigned to the new Memory-Block will be 
	// a "AllocationChunk". This means, this Chunks stores the
	// "original" Pointer to the MemBlock and is responsible for
	// "free()"ing the Memory later....
	if(!bAllocationChunkAssigned)
	{
		m_ptrLastChunk->IsAllocationChunk = true ;
		bAllocationChunkAssigned = true ;
	}
  }
  return RecalcChunkMemorySizePart(ptrNewChunks, uiChunkCount, m_sMemoryChunkSize*uiChunkCount) ;
  //return RecalcChunkMemorySize(m_ptrFirstChunk, m_uiMemoryChunkCount) ;
}

/******************
RecalcChunkMemorySize
******************/
inline bool MemoryPool::RecalcChunkMemorySize(SMemoryChunk *ptrChunk, unsigned int uiChunkCount)
{
  unsigned int uiMemOffSet = 0 ;
  for(unsigned int i = 0; i < uiChunkCount; i++)
  {
	  if(ptrChunk)
	  {
	    uiMemOffSet = (i * ((unsigned int) m_sMemoryChunkSize)) ;
	    ptrChunk->DataSize = (((unsigned int) m_sTotalMemoryPoolSize) - uiMemOffSet) ;
	    ptrChunk = ptrChunk->Next ;
	  }
	  else
	  {
		assert(false && "Error : ptrChunk == NULL") ;
        return false ;
	  }
  }
  return true ;
}

/******************
RecalcChunkMemorySize
******************/
inline bool MemoryPool::RecalcChunkMemorySizePart(SMemoryChunk *ptrChunk, unsigned int uiChunkCount, size_t ChunksMemoryBlockSize)
{
	unsigned int uiMemOffSet = 0 ;
	for(unsigned int i = 0; i < uiChunkCount; i++)
	{
		if(ptrChunk)
		{
			uiMemOffSet = (i * ((unsigned int) m_sMemoryChunkSize)) ;
			ptrChunk->DataSize = (((unsigned int) ChunksMemoryBlockSize) - uiMemOffSet) ;
			ptrChunk = ptrChunk->Next ;
		}
		else
		{
			assert(false && "Error : ptrChunk == NULL") ;
			return false ;
		}
	}
	return true ;
}

/******************
SetChunkDefaults
******************/
inline SMemoryChunk *MemoryPool::SetChunkDefaults(SMemoryChunk *ptrChunk)
{
  if(ptrChunk)
  {
    ptrChunk->Data = NULL ;
    ptrChunk->DataSize = 0 ;
    ptrChunk->UsedSize = 0 ;
	ptrChunk->IsAllocationChunk = false ;
    ptrChunk->Next = NULL ;
  }
  return ptrChunk ;
}

/******************
FindChunkHoldingPointerTo
******************/
inline SMemoryChunk *MemoryPool::FindChunkHoldingPointerTo(void *ptrMemoryBlock)
{
	SMemoryChunk *ptrTempChunk = m_ptrFirstChunk ;
	while(ptrTempChunk)
	{
		if(ptrTempChunk->Data == ptrMemoryBlock)
		{
			break ;
		}
		ptrTempChunk = ptrTempChunk->Next ;
	}
	return ptrTempChunk ;
}

/******************
FreeAllAllocatedMemory
******************/
inline void MemoryPool::FreeAllAllocatedMemory()
{
	SMemoryChunk *ptrChunk = m_ptrFirstChunk ;
	while(ptrChunk)
	{
		if(ptrChunk->IsAllocationChunk)
		{
			_aligned_free(((void *) (ptrChunk->Data))) ;
		}
		ptrChunk = ptrChunk->Next ;
	}
}

/******************
DeallocateAllChunks
******************/
inline void MemoryPool::DeallocateAllChunks()
{
  SMemoryChunk *ptrChunk = m_ptrFirstChunk ;
  SMemoryChunk *ptrChunkToDelete = NULL ;
  while(ptrChunk)
  {
	if(ptrChunk->IsAllocationChunk)
	{	
		if(ptrChunkToDelete)
		{
			_aligned_free(((void *) ptrChunkToDelete)) ;
		}
		ptrChunkToDelete = ptrChunk ;
	}
	ptrChunk = ptrChunk->Next ;
  }
}

/******************
IsValidPointer
******************/
inline bool MemoryPool::IsValidPointer(void *ptrPointer)
{
    SMemoryChunk *ptrChunk = m_ptrFirstChunk ;
	while(ptrChunk)
	{
		if(ptrChunk->Data == ((TByte *) ptrPointer))
		{
			return true ;
		}
		ptrChunk = ptrChunk->Next ;
	}
	return false ;
}

/******************
MaxValue
******************/
inline std::size_t MemoryPool::MaxValue(const std::size_t &sValueA, const std::size_t &sValueB) const
{
  if(sValueA > sValueB)
  {
	  return sValueA ;
  }
  return sValueB ;
}

}

void* operator new (size_t size)
{
	static MemPool::MemoryPool *pool = MemPool::MemoryPool::GetGlobalPool();
	return pool->GetMemory(size);
}

void operator delete (void* ptr)
{	
	if (NULL==ptr)
		return;
	static MemPool::MemoryPool *pool = MemPool::MemoryPool::GetGlobalPool();
    MemPool::SMemoryChunk *ptrChunk = pool->FindChunkHoldingPointerTo(ptr);
    if (ptrChunk)
	    return pool->FreeMemory(ptrChunk);
    return free(ptr);
}

void* operator new[] (size_t size)
{
	static MemPool::MemoryPool *pool = MemPool::MemoryPool::GetGlobalPool();
	return pool->GetMemory(size);
}

void operator delete[] (void* ptr)
{	
	if (NULL==ptr)
		return;
	static MemPool::MemoryPool *pool = MemPool::MemoryPool::GetGlobalPool();
	//return pool.FreeMemory(ptr);
    MemPool::SMemoryChunk *ptrChunk = pool->FindChunkHoldingPointerTo(ptr);
    if (ptrChunk)
        return pool->FreeMemory(ptrChunk);
    return free(ptr);
}


http://www.codeproject.com/Articles/27487/Why-to-use-memory-pool-and-how-to-implement-it

Memory allocation in C/C++ (via malloc or new) can take a lot of time.
To make things worse, the memory will fragment over time, so an application's
performance will decrease when it is running over a long period of time and/or 
performing a lot of memory (de-)allocations. Especially if you are allocating 
very small amounts of memory very often, the heap becomes fragmented.

A (possible) solution can be a Memory Pool.
A "Memory Pool" allocates a big amount of memory on startup, and will separate 
this block into smaller chunks. Every time you request memory from the pool,
it is taken from the previously allocated chunks, and not from the OS. 
The biggest advantages are:

Very little (to none) heap-fragmentation
Faster than the "normal" memory (de-)allocation (e.g., via malloc, new, etc.)

Additionally, you get these benefits:
Check if an arbitrary pointer is inside the Memory Pool
Write a "Heap-Dump" to your HDD (great for post-mortem debugging, etc.)
Some kind of "memory-leak detection": the Memory Pool will throw an assertion 
when you have not freed all previously allocated memory


Some background work 

#include <span class="code-keyword"><windows.h></span>
#include <span class="code-keyword"><iostream.h></span>

class CTestClass
{    
    char m_chBuf[4096];
};

int main()
{
    DWORD count = GetTickCount();    
    
    for(unsigned int i=0; i<0x5fffff; i++)
    {
        CTestClass *p = new CTestClass;
        delete p;
    }
    
    cout << "Interval = " << GetTickCount()-count << " ms" << endl;
    
    return 0;
} 

In my computer (Intel 3.0G Hz CPU / 512 MB of RAM / Win XP SP2 / VC6.0), the result is:
Interval = 8735 ms

#include <span class="code-keyword"><windows.h></span>
#include <span class="code-keyword"><iostream.h></span>

char buf[4100];  //Simple Memory Pool 

class CTestClass
{
    char m_chBuf[4096];
public:    
    void *operator new(unsigned int uiSize)
    {
        return (void *)buf;
    }
    void  operator delete(void *p)
    {
    }
};

int main()
{
    DWORD count = GetTickCount();
    
    for(unsigned int i=0; i<0x5fffff; i++)
    {
        CTestClass *p = new CTestClass; //Didn`t use system new operator!
        delete p;
    }
    
    cout << " Interval = " << GetTickCount()-count << " ms" << endl;
    
    return 0;
}

In my computer, the result is:
Interval = 391 ms



#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__
                                          
class CMemPool
{
private:
    //The purpose of the structure`s definition is that we can operate linkedlist conveniently
    struct _Unit                     //The type of the node of linkedlist.
    {
        struct _Unit *pPrev, *pNext;
    };

    void* m_pMemBlock;                //The address of memory pool.

    //Manage all unit with two linkedlist.
    struct _Unit*    m_pAllocatedMemBlock; //Head pointer to Allocated linkedlist.
    struct _Unit*    m_pFreeMemBlock;      //Head pointer to Free linkedlist.

    unsigned long    m_ulUnitSize; //Memory unit size. There are much unit in memory pool.
    unsigned long    m_ulBlockSize;//Memory pool size. Memory pool is make of memory unit.

public:
    CMemPool(unsigned long lUnitNum = 50, unsigned long lUnitSize = 1024);
    ~CMemPool();
    
    void* Alloc(unsigned long ulSize, bool bUseMemPool = true); //Allocate memory unit
    void Free( void* p );                                   //Free memory unit
};

#endif //__MEMPOOL_H__

/*==========================================================
CMemPool:
    Constructor of this class. It allocate memory block from system and create
    a static double linked list to manage all memory unit.

Parameters:
    [in]ulUnitNum
    The number of unit which is a part of memory block.

    [in]ulUnitSize
    The size of unit.
//=========================================================
*/
CMemPool::CMemPool(unsigned long ulUnitNum,unsigned long ulUnitSize) :
    m_pMemBlock(NULL), m_pAllocatedMemBlock(NULL), m_pFreeMemBlock(NULL), 
    m_ulBlockSize(ulUnitNum * (ulUnitSize+sizeof(struct _Unit))), 
    m_ulUnitSize(ulUnitSize)
{    
    m_pMemBlock = malloc(m_ulBlockSize);     //Allocate a memory block.
    
    if(NULL != m_pMemBlock)
    {
        for(unsigned long i=0; i<ulUnitNum; i++)  //Link all mem unit . Create linked list.
        {
            struct _Unit *pCurUnit = (struct _Unit *)( (char *)m_pMemBlock + i*(ulUnitSize+sizeof(struct _Unit)) );
            
            pCurUnit->pPrev = NULL;
            pCurUnit->pNext = m_pFreeMemBlock;    //Insert the new unit at head.
            
            if(NULL != m_pFreeMemBlock)
            {
                m_pFreeMemBlock->pPrev = pCurUnit;
            }
            m_pFreeMemBlock = pCurUnit;
        }
    }    
} 


/*===============================================================
~CMemPool():
    Destructor of this class. Its task is to free memory block.
//===============================================================
*/
CMemPool::~CMemPool()
{
    free(m_pMemBlock);
}

/*================================================================
Alloc:
    To allocate a memory unit. If memory pool can`t provide proper memory unit,
    It will call system function.

Parameters:
    [in]ulSize
    Memory unit size.

    [in]bUseMemPool
    Whether use memory pool.

Return Values:
    Return a pointer to a memory unit.
//=================================================================
*/
void* CMemPool::Alloc(unsigned long ulSize, bool bUseMemPool)
{
    if(    ulSize > m_ulUnitSize || false == bUseMemPool || 
        NULL == m_pMemBlock   || NULL == m_pFreeMemBlock)
    {
        return malloc(ulSize);
    }

    //Now FreeList isn`t empty
    struct _Unit *pCurUnit = m_pFreeMemBlock;
    m_pFreeMemBlock = pCurUnit->pNext;            //Get a unit from free linkedlist.
    if(NULL != m_pFreeMemBlock)
    {
        m_pFreeMemBlock->pPrev = NULL;
    }

    pCurUnit->pNext = m_pAllocatedMemBlock;
    
    if(NULL != m_pAllocatedMemBlock)
    {
        m_pAllocatedMemBlock->pPrev = pCurUnit; 
    }
    m_pAllocatedMemBlock = pCurUnit;

    return (void *)((char *)pCurUnit + sizeof(struct _Unit) );
}


/*================================================================
Free:
    To free a memory unit. If the pointer of parameter point to a memory unit,
    then insert it to "Free linked list". Otherwise, call system function "free".

Parameters:
    [in]p
    It point to a memory unit and prepare to free it.

Return Values:
    none
//================================================================
*/
void CMemPool::Free( void* p )
{
    if(m_pMemBlock<p && p<(void *)((char *)m_pMemBlock + m_ulBlockSize) )
    {
        struct _Unit *pCurUnit = (struct _Unit *)((char *)p - sizeof(struct _Unit) );

        m_pAllocatedMemBlock = pCurUnit->pNext;
        if(NULL != m_pAllocatedMemBlock)
        {
            m_pAllocatedMemBlock->pPrev = NULL;
        }

        pCurUnit->pNext = m_pFreeMemBlock;
        if(NULL != m_pFreeMemBlock)
        {
             m_pFreeMemBlock->pPrev = pCurUnit;
        }

        m_pFreeMemBlock = pCurUnit;
    }
    else
    {
        free(p);
    }
}

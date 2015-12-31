/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifdef USE_MEMPOOL

#include "GenericUtils/MemPool.h"

namespace gu
{

    void* MemPool::s_pMemPool = NULL;
    MemPool::Block* MemPool::s_pAllocatedMemBlock = NULL;
    MemPool::Block* MemPool::s_pFreeMemBlock = NULL;
    unsigned long MemPool::s_ulBlocksNum = 10000;
    unsigned long MemPool::s_ulBlockSize = 255;
    unsigned long MemPool::s_ulPoolSize = (s_ulBlocksNum * (s_ulBlockSize+sizeof(Block)));



    
    std::recursive_mutex gu::MemPool::s_mutexProtect;




    void MemPool::InitPool()
    {
        if(s_pMemPool == NULL)
        {    


            s_pMemPool = malloc(s_ulPoolSize);     //Allocate a memory block.
        
            if(s_pMemPool != NULL)
            {

                std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

                memset(s_pMemPool, 0, s_ulPoolSize);

                for(unsigned long i=0; i<s_ulBlocksNum; i++)  //Link all mem unit . Create linked list.
                {
                    Block *pCurUnit = (Block*)( (char*)s_pMemPool + i*(s_ulBlockSize+sizeof(Block)) );

                    pCurUnit->pPrev = s_pFreeMemBlock;
                    pCurUnit->pNext = NULL;    //Insert the NEW unit at head.

                    if(s_pFreeMemBlock != NULL)
                    {
                        s_pFreeMemBlock->pNext = pCurUnit;
                    }

                    s_pFreeMemBlock = pCurUnit;
                }
            
            }    
        }
    }

    void MemPool::InitPool(unsigned long ulUnitSize, unsigned long ulUnitsNum)
    {
        s_ulPoolSize = (ulUnitsNum * (ulUnitSize+sizeof(Block)));
        s_ulBlockSize = ulUnitSize;

        InitPool();
    }

    void MemPool::DeInitPool()
    {
        std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

        free(s_pMemPool);
    
    }

    MemPool::MemPool()
    {    
        MemPool::InitPool();
    }

    MemPool::~MemPool()
    {
    
    }


    void* MemPool::Alloc(unsigned long ulSize, bool bUseMemPool)
    {
        std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

        MemPool::InitPool();

        if((ulSize > s_ulBlockSize)
        || (bUseMemPool == false)
        || (s_pMemPool == NULL)
        || (s_pFreeMemBlock == NULL))
        {
            return malloc(ulSize);
        }

        //Now FreeList isn`t empty
        Block *pCurUnit = s_pFreeMemBlock;
        s_pFreeMemBlock = pCurUnit->pPrev;            //Get a unit from free linkedlist.
        if(s_pFreeMemBlock != NULL)
        {
            s_pFreeMemBlock->pNext = NULL;
        }

        pCurUnit->pPrev = s_pAllocatedMemBlock;

        if(s_pAllocatedMemBlock != NULL)
        {
            s_pAllocatedMemBlock->pNext = pCurUnit; 
        }
        s_pAllocatedMemBlock = pCurUnit;

        void* pointer = (void*)((char*)pCurUnit + sizeof(Block) );

        return pointer;
    }



    void MemPool::Free( void* p )
    {
        std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);


        if( (s_pMemPool < p) && (p < (void*)((char*)s_pMemPool + s_ulPoolSize)))
        {
            Block *pCurUnit = (Block*)( (char*)p - sizeof(Block) );

            s_pAllocatedMemBlock = pCurUnit->pPrev;
            if(s_pAllocatedMemBlock != NULL)
            {
                s_pAllocatedMemBlock->pNext = NULL;
            }

            pCurUnit->pPrev = s_pFreeMemBlock;
            if(s_pFreeMemBlock != NULL)
            {
                s_pFreeMemBlock->pNext = pCurUnit;
            }

            s_pFreeMemBlock = pCurUnit;

        
            return;
        }

    
        free(p);
    }



    void* IMemPool::operator new(size_t size)
    {
        void *p = MemPool::Alloc(size);
    
        if(p == NULL) 
        {
            throw "allocation fail : no free memory";
        }


        //gu::MemLeakDetector::Allocate( p, size, "file not specified", 0 );


        return p;
    }

    void IMemPool::operator delete(void* p)
    {

        //vsge::MemLeakDetector::Free( p );


        MemPool::Free(p);
    }




    void* IMemPool::operator new( size_t size, const char *file, int line )
    { 
        void *p = MemPool::Alloc(size);


        //vsge::MemLeakDetector::Allocate( p, size, file, line );


        return p;
    };

    void IMemPool::operator delete( void* p, const char *file, int line) 
    { 

        //vsge::MemLeakDetector::Free( p );


        MemPool::Free(p);
    };



    void* IMemPool::operator new[]( size_t size, const char *file, int line ) 
    { 
        void *p = MemPool::Alloc(size);

        //vsge::MemLeakDetector::Allocate( p, size, file, line );

        return p;
    }

    void IMemPool::operator delete[] ( void* p, const char *file, int line) 
    { 

        //vsge::MemLeakDetector::Free( p );


        MemPool::Free(p);
    };


} // namespace gu

#endif //USE_MEMPOOL

//--------------------------------------------------------------------------------
// MemPool.cpp
//
// Copyright (c) 2022, Cristian Vasile
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL CRISTIAN VASILE BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: Cristian Vasile
// File created on 03.2013
//--------------------------------------------------------------------------------


#include "MemPool/MemPool.h"

namespace mp
{
    void* MemPool::s_pMemPool = nullptr;
	MemPool::Block* MemPool::s_pAllocatedMemBlock = nullptr;
	MemPool::Block* MemPool::s_pFreeMemBlock = nullptr;
    unsigned long MemPool::s_ulBlocksNum = 10000;
    unsigned long MemPool::s_ulBlockSize = 255;
    unsigned long MemPool::s_ulPoolSize = 0;

    std::recursive_mutex mp::MemPool::s_mutexProtect;

  
    void MemPool::InitPool(unsigned long ulUnitSize, unsigned long ulUnitsNum)
    {
		std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

		if (s_pMemPool == nullptr)
		{
			s_ulBlocksNum = ulUnitsNum;
			s_ulBlockSize = ulUnitSize;
			s_ulPoolSize = (s_ulBlocksNum * (s_ulBlockSize + sizeof(Block)));
			

			s_pMemPool = malloc(s_ulPoolSize);     //Allocate a memory block.

			if (s_pMemPool != nullptr)
			{
				std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

				memset(s_pMemPool, 0, s_ulPoolSize);

				for (unsigned long i = 0; i<s_ulBlocksNum; i++)  //Link all mem unit . Create linked list.
				{
					Block *pCurUnit = (Block*)((char*)s_pMemPool + i*(s_ulBlockSize + sizeof(Block)));

					pCurUnit->pPrev = s_pFreeMemBlock;
					pCurUnit->pNext = nullptr;    //Insert the NEW unit at head.

					if (s_pFreeMemBlock != nullptr)
					{
						s_pFreeMemBlock->pNext = pCurUnit;
					}

					s_pFreeMemBlock = pCurUnit;
				}

			}
		}
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
		|| (s_pMemPool == nullptr)
		|| (s_pFreeMemBlock == nullptr))
        {
            return malloc(ulSize);
        }

        //Now FreeList isn`t empty
        Block *pCurUnit = s_pFreeMemBlock;
        s_pFreeMemBlock = pCurUnit->pPrev;            //Get a unit from free linkedlist.
		if (s_pFreeMemBlock != nullptr)
        {
			s_pFreeMemBlock->pNext = nullptr;
        }

        pCurUnit->pPrev = s_pAllocatedMemBlock;

		if (s_pAllocatedMemBlock != nullptr)
        {
            s_pAllocatedMemBlock->pNext = pCurUnit; 
        }
        s_pAllocatedMemBlock = pCurUnit;

        void* pointer = (void*)((char*)pCurUnit + sizeof(Block) );

        return pointer;
    }



    void MemPool::Free(void* p)
    {
        std::lock_guard<std::recursive_mutex> lock(s_mutexProtect);

        if( (s_pMemPool < p) && (p < (void*)((char*)s_pMemPool + s_ulPoolSize)))
        {
            Block *pCurUnit = (Block*)( (char*)p - sizeof(Block) );

            s_pAllocatedMemBlock = pCurUnit->pPrev;
			if (s_pAllocatedMemBlock != nullptr)
            {
				s_pAllocatedMemBlock->pNext = nullptr;
            }

            pCurUnit->pPrev = s_pFreeMemBlock;
			if (s_pFreeMemBlock != nullptr)
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
		
#ifdef USE_EXCEPTIONS
		if (p == nullptr)
        {
            throw "allocation fail : no free memory";
        }
#endif //USE_EXCEPTIONS
		
		return p;
    }

    void IMemPool::operator delete(void* p)
    {
        MemPool::Free(p);
    }




    void* IMemPool::operator new( size_t size, const char *file, int line )
    { 
        void *p = MemPool::Alloc(size);

        return p;
    };

    void IMemPool::operator delete( void* p, const char *file, int line) 
    { 
		MemPool::Free(p);
    };



    void* IMemPool::operator new[]( size_t size, const char *file, int line ) 
    { 
        void *p = MemPool::Alloc(size);
        return p;
    }

    void IMemPool::operator delete[] ( void* p, const char *file, int line) 
    { 
        MemPool::Free(p);
    };

} // namespace gu


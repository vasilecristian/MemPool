/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef MEMPOOL_BASE_H
#define MEMPOOL_BASE_H

#include <mutex>


namespace mp
{
	/**
	* This is an Interface. If you want a Class to use 'Memory Pool' you need to extend IMemPool.
	* The IMemPool have only operators functions: operator new and operator delete, that will use the
	* memory pool to allocate the required memory.*/
	class IMemPool
	{
	public:
		void* operator new(size_t size);
		void operator delete(void* p);


		void* operator new(size_t size, const char *file, int line);
		void operator delete(void* p, const char *file, int line);

		void* operator new[](size_t size, const char *file, int line);
		void operator delete[](void* p, const char *file, int line);
	};


   /** This is the implementation of a memory pool. */
	class MemPool
	{
		friend class IMemPool;
	public:

	   /**
		* This function will initialize the pool buffer. If this 
		* function will not be called the pool will be initialized with a default size when the 
		* Alloc function will be called for the first time.
		*
		* @param ulUnitSize an unsigned long which means the size of an (minimum) block unit (in bytes)
		* @param ulUnitsNum an unsigned long which is means how many units to generate.
		*/
		static inline void InitPool(unsigned long ulUnitSize = 255, unsigned long ulUnitsNum = 10000);

	   /** This function will free the pool buffer. */
		static void DeInitPool();


	private:
	   /**
		* Allocate memory unit
		*
		* @param ulSize an unsigned long representing the size of needed memory.
		* @param bUseMemPool a bool with default value true, used to specify if MemPool will be used or not.
		* @return the address of an allocated memory chunk.
		*/
		static inline void* Alloc(unsigned long ulSize, bool bUseMemPool = true);

	   /**
		* Free memory unit
		*
		* @param p a pointer to the memory that must be freed
		*/
		static inline void Free( void* p );

	private:

		/** The purpose of the structure`s definition is that
		* we can operate linked list conveniently */
		struct Block
		{
			/** the previous Block */
			Block *pPrev;

			/** the next Block */
			Block *pNext;
		};


		static std::recursive_mutex s_mutexProtect;


		/** The address of memory pool. */
		static void* s_pMemPool;


		/**
		* Head pointer to Allocated linkedlist.
		* when a memory bloc is alocated will be linked to this list, and unlinked from
		* the s_pFreeMemBlock.*/
		static Block* s_pAllocatedMemBlock;

		/**
		* Head pointer to Free linkedlist. At the beginning all the memory blocks are
		* added to this list.*/
		static Block* s_pFreeMemBlock;


		/**
		* Memory unit size. There are much unit in memory pool.
		* The size is in bytes.*/
		static unsigned long s_ulBlockSize;

		/**
		* The number of the memory blocks.
		* The total amount of memory allocated is the (s_ulBlocksNum * (s_ulBlockSize+sizeof(Block))); .*/
		static unsigned long s_ulBlocksNum;

		/**
		* Memory pool size. Memory pool is make of memory unit.
		* The total amount of memory allocated is the (s_ulBlocksNum * (s_ulBlockSize+sizeof(Block))); .*/
		static unsigned long s_ulPoolSize;


	private:

		/** The constructor. */
		MemPool();

		/** The destructor. */
		virtual ~MemPool();
	};

} // namespace mp

#endif //MEMPOOL_BASE_H

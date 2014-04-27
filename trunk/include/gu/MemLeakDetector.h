/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/
#ifndef MEMLEAKDETECTOR_BASE_H
#define MEMLEAKDETECTOR_BASE_H

#include <iostream>
#include <fstream>
#include <map>

#include "gu/Singleton.hpp"
#include "gu/Threads.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif


namespace gu
{
    /**
     * The effective allocator
     *
     * @param size is the size in bytes of the needed memory block.
     * @return an void pointer for the allocated memory block.
     */
#if (USE_CUSTOM_ALLOCATORS)
    void* Allocator( std::size_t size);
#else //USE_CUSTOM_ALLOCATORS
	inline void* Allocator( std::size_t size)
	{
		void* p = malloc(size);

		if(!p)
			return NULL;

		memset(p, 0, size);

		return p;
	};
#endif //USE_CUSTOM_ALLOCATORS


    /**
     * The efective function that will free the mem
     *
     * @param ptr a void* representing the object pointer that must be deleted.
     */
#if (USE_CUSTOM_ALLOCATORS)
    void DeAllocator( void* ptr );
#else //USE_CUSTOM_ALLOCATORS
	inline void DeAllocator( void* ptr )
	{
		 free(ptr);
	}
#endif //USE_CUSTOM_ALLOCATORS

    

    /**
     * Special malloc used to record the allocation.
     *
     * @param size is the size in bytes of the needed memory block.
     * @param file is a chat* pointer with the name of the file (used to store the __FILE__)
     * @param line will stote the line number (__LINE__) in the "file" (the parameter above)
     * @return an void pointer for the allocated memory block.
     */
    void*	Malloc( std::size_t size, const char *file, int line );

    /**
     * Special custom free memory.
     *
     * @param ptr a void* representing the object pointer that must be deleted.
     */
    void	Free( void* ptr );


    /**
     * Used to track the memory leaks. This is a singleton type class that will store all
     * the allocated pointers, size, line and file. The constructor and destructor are
     * protected because this is a singleton. Use GetInstance() instead.
     * The file and line number with memory leaks will be printed in a file when the PrintStatus()
     * is called.
     */
    class MemLeakDetector
    {

    public:

        /** This will initialize the class members. This must be called before Begin(). */
        static void Initialize();

        /** This will start the recording. */
        static void Begin();

        /** This will stop the recording. */
        static void End();

        /**
         * Set the filename of the output. Is not mandatory to call this function.
         * There is already a default name.
         *
         * @param fileName is the file name for the output file.
         */
        static void SetOutputFileName(const string& fileName);


        /**
         * Use it to record the possible 'leak' info (in to the map).
         *
         * @param ptr is the pointer to the allocated memory block.
         * @param size is the size of allocated memory block.
         * @param fileName is the source file name where the allocation occurred.
         * @param line is the line number in the 'fileName' (the previous parameter)
         */
        static void Allocate( const void* ptr, const size_t size, const char *fileName, int line);

        /**
         * Used to remove a record from the map.
         *
         * @param ptr a pointer to the memory that must be freed.
         * @return bool - true if the pointer was removed, and false if not.
         */
        static bool Free( const void* ptr);


        /**
         * This function will set an label for all memory allocation that will occurs from now on 
         * the current thread. To reset it use the ResetProfileMessage()
         * @param msg is the profile name.
         */
        static void SetProfileMessage(const char* msg);

        /**
         * This function will set an label for all memory allocation that will occurs from now on 
         * the current thread to "" aka empty string.
         */
        static void ResetProfileMessage();

        /** Class used to implement the Scope profile manager */
        class ScopeProfileMessage
        {
        public:
            /** the constructor */
            explicit ScopeProfileMessage(const char* msg) { SetProfileMessage(msg); }

            /** the destructor */
            ~ScopeProfileMessage() { ResetProfileMessage(); }
        };


        /**
         * Enable or disable the tracker. This is used almost internally, not to track the internal allocations.
         *
         * @param enable can be true or false.
         */
        static void Enable(bool enable);


        /** Print to the output file the content of map. */
        static void PrintStatus();

    protected:



        /** The mutex that will protect the static functions for multithreading access.*/
        static std::recursive_mutex s_mutexProtect;



        /**
         * This is used to profile better the memory allocations. So this map is used to store the some labels 
         * for each thread, for some functions or piece of code. The memory leaks log file will record this label for 
         * each leak, so the leak can be identify better.
         */
        static std::map<ThreadID, std::string> s_messageByThread;

        /** Used to store the status for each thread. By default the recording is enabled for each thread. This map is
         *  used to allow the user toenable or disable the recording for a specific thread.
         */
        static std::map<ThreadID, bool> s_enableByThread;

        /** Used to specify if class is initialized or not */
        static bool m_initialized;

        /** Used to specify if recording is initialized or not */
        static bool m_started;

        /**
         * The constructor is protected because this is a singleton.
         * use GetInstance() instead.
         */
        MemLeakDetector();

        /**
         * The destructor is protected because this is a singleton.
         * use GetInstance() instead.
         */
        virtual ~MemLeakDetector();

    private:


        /**
         * This is a helper class that will be used to store info about a pointer.
		 * Will store the size of the allocated memory, the source code filename where the allocation
		 * occured, and the line in that file.
         */
        class AllocUnit
        {
        public:

            /** the constructor */
            AllocUnit(size_t size, 
                      const char *fileName, 
                      int line, 
                      ThreadID threadID = ThreadID(), 
                      std::string msg = ""): m_size(size)
                                           , m_fileName(fileName)
                                           , m_line(line)
                                           , m_threadID(threadID)
                                           , m_msg(msg){}

            /** the destructor */
            ~AllocUnit(){}

            /** the size of the allocated memory */
            size_t m_size;

            /** the source file name where allocation occured */
            const char *m_fileName;

            /** the line number in the source file where allocation occured */
            int m_line;

            /** this is the id of the thread where the allocation occured.*/
            ThreadID m_threadID;

            /** this is the profile message. Is optional and is used to identify with accuracy where the allocation occured. */
            std::string m_msg;
        };



        /**
         * The filename where the output will be printed.
		 * This file will contine only the leaks.
         */
        static string s_leaksFilename;


		/** the filename with all allocation and deallocation occured. */
        static string s_logFilename;


		/** this is the stream used for s_logFilename. */
		static ofstream* s_log;

		/**
         * Write to info file about freed memory in the log file.
		 * @param ptr is a pointer to the allocated memory block.
		 * @param size is the size of allocated block.
		 * @param line is the line number in the source file where the free occured.
		 * @param file is the filename of the source file where the free occured.
         */
		static void LogFree(const void* ptr, int size, int line, string& file);

		/**
         * Write to info file about allocated memory in the log file.
		 * @param ptr is a pointer to the allocated memory block.
		 * @param size is the size of allocated block.
		 * @param line is the line number in the source file where the allocation occured.
		 * @param file is the filename of the source file where the allocation occured.
         */
		static void LogAlloc(const void* ptr, int size, int line, string& file);

        /**
         * Write to info file a message regarding the internal error occured.
         * @param message is the error message.
         */
        static void LogError(string& message);

        /**
         * This will store all allocations. Calling NEW an entry will be added. Calling delete that entry will be removed.
         * At a certain point if you have something in map, means that are valid pointers (aka memory leaks)
         */
        static map<const void *, AllocUnit*> s_memoryMap;

        /** the size in bytes of all the allocated memories */
        static size_t m_totalSize;

    };
}

//void* operator new(size_t size);
void* operator new( size_t size, const char *file, int line );
void operator delete( void* p, const char *file, int line) ;
void operator delete( void* p);

//void* operator new[](size_t size);
void* operator new[]( size_t size, const char *file, int line );
void operator delete[] ( void* p, const char *file, int line) ;
void operator delete[] ( void* p);


#define	NEW new(__FILE__, __LINE__)  //use this to call the new operator
#define	malloc(size) vsge::Malloc(size, __FILE__, __LINE__)
#define	free(pointer) vsge::Free(pointer)



#endif //MEMLEAKDETECTOR_BASE_H

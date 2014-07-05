/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/
#ifndef MEMLEAKDETECTOR_BASE_H
#define MEMLEAKDETECTOR_BASE_H

#if defined(USE_MEMLEAKDETECTOR) && USE_MEMLEAKDETECTOR

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <atomic>
#include <mutex>
#include <cstring>
#include "gu/Threads.h"

#ifdef _MSC_VER
#include <crtdbg.h>
#endif


namespace gu
{
    /**
     * The effective allocator. You can overwrite this if you 
     * define USE_CUSTOM_ALLOCATORS=1 .
     *
     * @param size is the size in bytes of the needed memory block.
     * @return an void pointer for the allocated memory block.
     */
#if (USE_CUSTOM_ALLOCATORS)
    void* Allocator( std::size_t size);
#else //USE_CUSTOM_ALLOCATORS
    inline void* Allocator( std::size_t size, bool corruptionCheck = false)
	{
        void* p = malloc(size);

		if(!p)
			return NULL;

		memset(p, 0, size);

		return p;
    }
#endif //USE_CUSTOM_ALLOCATORS


    /**
     * The efective function that will free the mem. 
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
    void* Malloc( std::size_t size, const char *file, int line );

    /**
     * Special custom free memory.
     *
     * @param ptr a void* representing the object pointer that must be deleted.
     */
    void Free( void* ptr );


    /**
     * Used to track the memory leaks. This is a class that will store all
     * the allocated pointers, size, line and file. The constructor and destructor are
     * protected because all functions are static.
     * The file and line number with memory leaks will be printed in a file when the PrintStatus()
     * is called.
     * How to use it? Take a look at the following example:
     *
     * #define USE_MEMLEAKDETECTOR 1
     * #include <gu/MemLeakDetector.h>
     *
     * int main(int argc, char *argv[])
     * {
     *      // Activate the leak detection
     *      MEMLEAKDETECTOR_BEGIN;
     *      
     *      // Using it without profile message:
     *      int* a = NEW int(0);
     *
     *      // Using it with profile message
     *      MEMLEAKDETECTOR_SET_PROFILE_MSG("Test1");
     *      int* b = NEW int(0);
     *      MEMLEAKDETECTOR_REMOVE_PROFILE_MSG;
     *
     *      // Using it with scope profile message
     *      {
	 *          MEMLEAKDETECTOR_SET_SCOPE_PROFILE_MSG("Test2");
	 *          int* c = NEW int(0);
     *      }
     *       
     *      // Stop leak detection
     *      MEMLEAKDETECTOR_END;
     *
     *      // This will print the status
     *      MEMLEAKDETECTOR_PRINT_STATUS;
     *
     *      return 0;
     * }
     */
    class MemLeakDetector
    {

    public:

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
        static void SetOutputFileName(const std::string& fileName);


        /**
         * Use it to record the possible 'leak' info (in to the map).
         * @param ptr is the pointer to the allocated memory block.
         * @param size is the size of allocated memory block.
         * @param fileName is the source file name where the allocation occurred.
         * @param line is the line number in the 'fileName' (the previous parameter)
         */
        static void Allocate( const void* ptr, const size_t size, const char *fileName, int line);

        /**
         * Used to remove a record from the map.
         * @param ptr a pointer to the memory that must be freed.
         * @return bool - true if the pointer was removed, and false if not.
         */
        static bool Free( const void* ptr);


        /**
         * This function will set an label for all memory allocation that will occurs from now, on 
         * the current thread. To reset it use the ResetProfileMessage()
         * @param msg is the profile name.
         */
        static void SetProfileMessage(const char* msg);

        /**
         * This function will set "" (aka empty string) label for all 
         * memory allocation that will occurs from now, on the current
         * thread.
         */
        static void RemoveProfileMessage();

        /** Class used to implement the Scope profile manager */
        class ScopeProfileMessage
        {
        public:
            /** the constructor */
            explicit ScopeProfileMessage(const char* msg) { SetProfileMessage(msg); }

            /** the destructor */
            ~ScopeProfileMessage() { RemoveProfileMessage(); }
        };


        /**
         * Enable or disable the leak detection for the current thread.
         * @param enable can be true or false.
         */
        static void Enable(bool enable);


        /** Print to the output file the content of map. */
        static void PrintStatus();

        /** Use this to activate the corruption check
         * This must be used only at the beginning.
         */
        static void EnableCorruptionCheck();

        /**
         * @brief Return true if the corruptioncheck was enabled.
         */
        static bool IsCorruptionCheckEnabled();


        static void CorruptionCheck();

    protected:

        /** The mutex that will protect the static functions for multithreading access.*/
        static std::recursive_mutex s_mutexProtect;

        /**
         * This is used to profile better the memory allocations. So this map is used to store some labels 
         * for each thread, for some functions or piece of code. The memory leaks log file will record this label for 
         * each leak, so the leak can be identify better.
         */
        static std::map<ThreadID, std::string> s_messageByThread;

        /** Used to store the status for each thread. By default the recording is enabled for each thread. This map is
         *  used to allow the user toenable or disable the recording for a specific thread.
         */
        static std::map<ThreadID, bool> s_enableByThread;



        /** Used to specify if recording is initialized or not */
        static std::atomic<bool> s_started;

        /** This is used to activate the corruption check */
        static std::atomic<bool> s_corruptionCheck;

        /**
         * The constructor is protected because all members are static functions.
         * So you do not need to use it.
         */
        MemLeakDetector();

        /**
         * The destructor is protected.
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
                      std::string msg = "")
                      : m_size(size)
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
        static std::string s_leaksFilename;


        /**
         * This will store all allocations. Calling NEW an entry will be added. Calling delete that entry will be removed.
         * At a certain point if you have something in map, means that are valid pointers (aka memory leaks)
         */
        static std::map<const char *, AllocUnit*> s_memoryMap;

        /** the size in bytes of all the allocated memories */
        static size_t s_totalSize;

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


#if !defined(MEMLEAKDETECTOR_LEAKS_FILENAME)
#define MEMLEAKDETECTOR_LEAKS_FILENAME              "MemLeaks.log"
#endif //MEMLEAKDETECTOR_LEAKS_FILENAME




#define	NEW new(__FILE__, __LINE__)  
#define	malloc(size) gu::Malloc(size, __FILE__, __LINE__)
#define	free(pointer) gu::Free(pointer)

#define MEMLEAKDETECTOR_BEGIN                       gu::MemLeakDetector::Begin();
#define MEMLEAKDETECTOR_END                         gu::MemLeakDetector::End();
#define MEMLEAKDETECTOR_PRINT_STATUS                gu::MemLeakDetector::PrintStatus();
#define MEMLEAKDETECTOR_SET_PROFILE_MSG(msg)        gu::MemLeakDetector::SetProfileMessage(msg);
#define MEMLEAKDETECTOR_REMOVE_PROFILE_MSG          gu::MemLeakDetector::RemoveProfileMessage();
#define MEMLEAKDETECTOR_SET_SCOPE_PROFILE_MSG(msg)  gu::MemLeakDetector::ScopeProfileMessage scope(msg);

#define MEMLEAKDETECTOR_CORRUPTION_CHECK_ENABLE     gu::MemLeakDetector::EnableCorruptionCheck();
#define MEMLEAKDETECTOR_CORRUPTION_CHECK            gu::MemLeakDetector::CorruptionCheck();

#else //not defined USE_MEMLEAKDETECTOR

#define	NEW new

#define MEMLEAKDETECTOR_BEGIN 
#define MEMLEAKDETECTOR_END 
#define MEMLEAKDETECTOR_PRINT_STATUS
#define MEMLEAKDETECTOR_SET_PROFILE_MSG(msg)
#define MEMLEAKDETECTOR_REMOVE_PROFILE_MSG
#define MEMLEAKDETECTOR_SET_SCOPE_PROFILE_MSG(msg)

#define MEMLEAKDETECTOR_CORRUPTION_CHECK_ENABLE
#define MEMLEAKDETECTOR_CORRUPTION_CHECK

#endif //USE_MEMLEAKDETECTOR

#endif //MEMLEAKDETECTOR_BASE_H

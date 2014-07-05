/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#if defined(USE_MEMLEAKDETECTOR) && USE_MEMLEAKDETECTOR
// Activate USE_MEMLEAKDETECTOR on source file, 
// so the library will have the functions.

#include "gu/MemLeakDetector.h"
#include "gu/Utils.h"
#include "gu/Log.h"

#ifndef _MSC_VER
#define NOEXCEPT noexcept
#else
#define NOEXCEPT
#endif


using namespace std;

/**
 * I must undef the malloc, because i want to use the system malloc
 */
#ifdef malloc
    #undef malloc
#endif

/**
 * I must undef the free, because i want to use the system free
 */
#ifdef free
    #undef free
#endif





//void* operator new(size_t size) 
//{ 
//    return vsge::Malloc(size, "file_name_not_available", 0);
//} 
void* operator new( size_t size, const char *file, int line ) 
{ 
    return gu::Malloc(size, file, line); 
}
void operator delete( void* p, const char *file, int line) 
{ 
    gu::Free(p); 
}
void operator delete( void* p) NOEXCEPT
{ 
    gu::Free(p); 
}


//void* operator new[](size_t size) 
//{ 
//    return vsge::Malloc(size, "file_name_not_available", 0);
//} 
void* operator new[]( size_t size, const char *file, int line ) 
{ 
    return gu::Malloc(size, file, line); 
}
void operator delete[] ( void* p, const char *file, int line) 
{ 
    gu::Free(p); 
}
void operator delete[] ( void* p) NOEXCEPT
{
    gu::Free(p); 
}



#define VALUE_TO_DETECT 1
void* gu::Malloc( std::size_t size, const char *file, int line )
{
	if(size == 0) 
        return NULL;	

    void* p = Allocator(size + (gu::MemLeakDetector::IsCorruptionCheckEnabled() ? size : 0) );

    if(gu::MemLeakDetector::IsCorruptionCheckEnabled())
    {
        memset((void*)((char*)p + size), VALUE_TO_DETECT, size);
    }

	if(!p) 
		return NULL;


    gu::MemLeakDetector::Allocate( p, size, file, line );


    return p;
}


void gu::Free( void* p )
{
    gu::MemLeakDetector::Free( p );

	DeAllocator(p);
}


std::string gu::MemLeakDetector::s_leaksFilename = MEMLEAKDETECTOR_LEAKS_FILENAME;

std::atomic<bool> gu::MemLeakDetector::s_started(false);

std::atomic<bool> gu::MemLeakDetector::s_corruptionCheck(false);

map<const char *, gu::MemLeakDetector::AllocUnit*> gu::MemLeakDetector::s_memoryMap;

map<ThreadID, std::string> gu::MemLeakDetector::s_messageByThread;

std::map<ThreadID, bool> gu::MemLeakDetector::s_enableByThread;

size_t gu::MemLeakDetector::s_totalSize = 0;

std::recursive_mutex gu::MemLeakDetector::s_mutexProtect;


void gu::MemLeakDetector::Begin()
{
    s_started = true;
}

void gu::MemLeakDetector::End()
{
    s_started = false;
}

void gu::MemLeakDetector::EnableCorruptionCheck()
{
    s_corruptionCheck= true;
}

bool gu::MemLeakDetector::IsCorruptionCheckEnabled()
{
    return s_corruptionCheck;
}


gu::MemLeakDetector::MemLeakDetector()
{
    std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    s_memoryMap.clear();
}

gu::MemLeakDetector::~MemLeakDetector() 
{   
	std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);
    
    s_enableByThread[GetThreadID()] = false;

    for( auto it = s_memoryMap.begin(); it != s_memoryMap.end(); it++ ) 
    {
        gu::MemLeakDetector::AllocUnit* unit =  (gu::MemLeakDetector::AllocUnit*)it->second;
        delete unit;
    }

    s_memoryMap.clear();
    s_totalSize = 0;
}



void gu::MemLeakDetector::SetOutputFileName(const string& fileName) 
{
	std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);
    
    gu::MemLeakDetector::s_leaksFilename = fileName;
}


void gu::MemLeakDetector::Allocate( const void* ptr, const size_t size, const char *fileName, int line) 
{
    // Check if the leak detector was started. If not, then return.
    if(!s_started) return;
    
    // Protect it for multithread access.
    std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    // Get the thread id, that is used to check if 
    // leak detection was disable for this thread.
    ThreadID tid = GetThreadID();

    // Check if this thread was added to the map. 
    // If not then add it.
    // If is added, then check if the leak detection was disabled 
    // for it.
    if(s_enableByThread.find(tid) == s_enableByThread.cend())
        s_enableByThread[tid] = true;
    else if(!s_enableByThread[tid])
        return;
    
    // Disable the leack detection for all internal 
    // allocations for this thread.
    s_enableByThread[tid] = false; 

    // Create the allocation unit with info about size, filename, line, thread id, etc...
    AllocUnit *unit = new AllocUnit(size, fileName, line, tid, s_messageByThread[tid]);
	
    // Insert the allocation unit in to the map
    s_memoryMap.insert( pair<const char*, gu::MemLeakDetector::AllocUnit*>( (char*)ptr, unit ) );

    // Increment the total allocated size
    s_totalSize += size;

    // Enable the leack detection for this thread.
    s_enableByThread[tid] = true;
}


bool gu::MemLeakDetector::Free( const void* ptr) 
{
    if(!s_started) 
    {
        return false;
    }

    std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    ThreadID tid = GetThreadID();

    if(s_enableByThread.find(tid) == s_enableByThread.end())
    {
        s_enableByThread[tid] = true;
    }
    else if(!s_enableByThread[tid])
    {
        return false;
    }

    if(s_memoryMap.size() == 0) return false;

    bool removed = false;

    s_enableByThread[tid] = false;
    if(s_memoryMap.find((char*)ptr) != s_memoryMap.end())
    {
        AllocUnit* unit = s_memoryMap[(char*)ptr];
        
        s_memoryMap.erase((char*)ptr);
		
        s_totalSize -= unit->m_size;

        delete unit;
        removed = true;
        
    }
    else
    {
        //this case is for pointers to objects, that are inside of an bigger object
        //the case of multiple inheritance!
        
        s_memoryMap[(char*)ptr] = NULL;
        auto it = s_memoryMap.find((char*)ptr);

        --it;    
        if(it != s_memoryMap.end())
        {
            const void* p = (*it).first;
            AllocUnit* unit = (*it).second;

            bool found = false;
            if(ptr == p)
                found = true;
            else
            {
                char* pc = (char*)p;
                char* ptrc = (char*)ptr;
                unsigned int offset = (ptrc - pc);
                if(offset < unit->m_size) found = true;
            }

            if(found)
            {
                s_memoryMap.erase((char*)p);
				
                s_totalSize -= unit->m_size;

                delete unit;
                removed = true;
            }
        }
        

        s_memoryMap.erase((char*)ptr);

    }
    s_enableByThread[tid] = true;

    return removed;
}

void gu::MemLeakDetector::SetProfileMessage(const char* msg)
{
    std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    s_messageByThread[GetThreadID()] = std::string(msg);
}

void gu::MemLeakDetector::RemoveProfileMessage()
{
    gu::MemLeakDetector::SetProfileMessage("");
}

void gu::MemLeakDetector::Enable(bool enable)
{
	std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    s_enableByThread[GetThreadID()] = enable;
}


void gu::MemLeakDetector::PrintStatus() 
{
	std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    if(s_leaksFilename.size() != 0)
    {   
        // Open the log file for output
        ofstream out( s_leaksFilename.c_str() );
        
        if(out.is_open())
        {        
            out << "Total Number of Pointers: " << s_memoryMap.size() << endl;
            out << "Total Size Allocated: " << s_totalSize << endl;
            
            out << endl << endl;
            
            out << "Pointer Information:" << endl;
            //map< const void*, vsge::MemLeakDetector::AllocUnit* >::iterator it;
            for( auto it = s_memoryMap.begin(); it != s_memoryMap.end(); it++ ) 
            {
                gu::MemLeakDetector::AllocUnit* unit =  (gu::MemLeakDetector::AllocUnit*)it->second;

                if(unit)
                    out << "Address: " << it->first << " | Thread: " << unit->m_threadID << " | Size: " << unit->m_size << "\t\t | file: " << unit->m_fileName << "(" << unit->m_line << ")" << " | Message: " << unit->m_msg << endl;
            }
			
            out.close();
        }
    }

}

void gu::MemLeakDetector::CorruptionCheck()
{
    if(!s_corruptionCheck) return;

    std::lock_guard<std::recursive_mutex> lock(gu::MemLeakDetector::s_mutexProtect);

    if(s_leaksFilename.size() != 0)
    {

        for( auto it = s_memoryMap.begin(); it != s_memoryMap.end(); it++ )
        {
            gu::MemLeakDetector::AllocUnit* unit =  (gu::MemLeakDetector::AllocUnit*)it->second;

            if(unit)
            {
                for(size_t i = 0; i<unit->m_size; i++)
                {
                    void* ptr = (char*)(it->first + unit->m_size + i);
                    if((*((char*)ptr)) != VALUE_TO_DETECT)
                    {
                        std::stringstream ss;
                        ss << "Memory corruption at Address: " << ptr
                           << " variable declared in file: "
                           << unit->m_fileName
                           << "(" << unit->m_line << ")";
                        gu::GUAssert (false, ss.str().c_str(), (char*)unit->m_fileName, unit->m_line );

                    }
                }
            }

        }

    }
}

#endif //USE_MEMLEAKDETECTOR
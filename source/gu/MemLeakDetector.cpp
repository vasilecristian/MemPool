/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/
#include "gu/MemLeakDetector.h"


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
};
void operator delete( void* p, const char *file, int line) 
{ 
    gu::Free(p); 
};
void operator delete( void* p)
{ 
    gu::Free(p); 
};


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
};
void operator delete[] ( void* p)
{
    gu::Free(p); 
};




void* gu::Malloc( std::size_t size, const char *file, int line )
{
	if(size == 0) 
        return NULL;	

	void* p = Allocator(size);

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


#define MLD_THREAD_LOCK_SCOPE  THREAD_MUTEX_RECURSIVE_LOCK_SCOPE(gu::MemLeakDetector::s_mutexProtect)



std::string gu::MemLeakDetector::s_leaksFilename = "MemLeaks.log";
std::string gu::MemLeakDetector::s_logFilename = "MemOperations.log";
ofstream* gu::MemLeakDetector::s_log = NULL;
bool gu::MemLeakDetector::m_initialized = false;
bool gu::MemLeakDetector::m_started = false;
map<const void *, gu::MemLeakDetector::AllocUnit*> gu::MemLeakDetector::s_memoryMap;
map<ThreadID, std::string> gu::MemLeakDetector::s_messageByThread;
std::map<ThreadID, bool> gu::MemLeakDetector::s_enableByThread;
size_t gu::MemLeakDetector::m_totalSize = 0;


std::recursive_mutex gu::MemLeakDetector::s_mutexProtect;


void gu::MemLeakDetector::Initialize()
{
    if(!m_initialized)
    {


        if(s_log == NULL) 
		    s_log = new ofstream(s_logFilename);

        m_initialized = true;
    }
}

void gu::MemLeakDetector::Begin()
{
    MLD_THREAD_LOCK_SCOPE;

    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

    m_started = true;
}

void gu::MemLeakDetector::End()
{
    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

    MLD_THREAD_LOCK_SCOPE;

    m_started = false;
}



gu::MemLeakDetector::MemLeakDetector()
{
    s_memoryMap.clear();
}


gu::MemLeakDetector::~MemLeakDetector() 
{   
    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

	MLD_THREAD_LOCK_SCOPE;
    
    s_enableByThread[GetThreadID()] = false;

    for( auto it = s_memoryMap.begin(); it != s_memoryMap.end(); it++ ) 
    {
        gu::MemLeakDetector::AllocUnit* unit =  (gu::MemLeakDetector::AllocUnit*)it->second;
        delete unit;
    }

    s_memoryMap.clear();
    m_totalSize = 0;
}



void gu::MemLeakDetector::SetOutputFileName(const string& fileName) 
{
    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

	MLD_THREAD_LOCK_SCOPE;
    
    gu::MemLeakDetector::s_leaksFilename = fileName;
    
}


void gu::MemLeakDetector::Allocate( const void* ptr, const size_t size, const char *fileName, int line) 
{

    if(!m_initialized || !m_started) 
    {
        return;
    }

    MLD_THREAD_LOCK_SCOPE;

    ThreadID tid = GetThreadID();

    if(s_enableByThread.find(tid) == s_enableByThread.cend())
    {
        s_enableByThread[tid] = true;
    }
    else if(!s_enableByThread[tid])
        return;
    

    /// Stop the allocation recording(only for this thread) for internal new 
    s_enableByThread[tid] = false; 

    AllocUnit *unit = new AllocUnit(size, fileName, line, tid, s_messageByThread[tid]);
	LogAlloc(ptr, unit->m_size, unit->m_line, string(unit->m_fileName));
    s_memoryMap.insert( pair<const void*, gu::MemLeakDetector::AllocUnit*>( ptr, unit ) );
    m_totalSize += size;

    s_enableByThread[tid] = true;


}


bool gu::MemLeakDetector::Free( const void* ptr) 
{
#if USE_VSGE_MEMORY_LEAK_TRACKER

    if(!m_initialized || !m_started) 
    {
        return false;
    }
    
    MLD_THREAD_LOCK_SCOPE;

    ThreadID tid = GetThreadID();

    if(s_enableByThread.find(tid) == s_enableByThread.end())
    {
        s_enableByThread[tid] = true;
    }
    else if(!s_enableByThread[tid])
        return false;

    bool removed = false;

    s_enableByThread[tid] = false;
    if(s_memoryMap.find(ptr) != s_memoryMap.end())
    {
        AllocUnit* unit = s_memoryMap[ ptr ];
        
        s_memoryMap.erase( ptr );
		LogFree(ptr, unit->m_size, unit->m_line, string(unit->m_fileName));
        m_totalSize -= unit->m_size;

        delete unit;
        removed = true;
        
    }
    else
    {
        //this case is for pointers to objects, that are inside of an bigger object
        //the case of multiple inheritance!
        
        s_memoryMap[ptr] = NULL;
        auto it = s_memoryMap.find(ptr);

        --it;    
        if(it != s_memoryMap.end())
        {
            const void* p = (*it).first;
            AllocUnit* unit = (*it).second;


            char* pc = (char*)p;
            char* ptrc = (char*)ptr;
            unsigned int offset = (ptrc - pc);
            if(offset < unit->m_size)
            {
                s_memoryMap.erase( p );
				LogFree(p, unit->m_size, unit->m_line, string(unit->m_fileName));
				LogFree(ptr, unit->m_size, unit->m_line, string(unit->m_fileName));
                m_totalSize -= unit->m_size;

                delete unit;
                removed = true;
            }
        }
        

        s_memoryMap.erase( ptr );		

    }
    s_enableByThread[tid] = true;

    return removed;

#else

	return false;

#endif //USE_VSGE_MEMORY_LEAK_TRACKER
}

void gu::MemLeakDetector::SetProfileMessage(const char* msg)
{
    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

    MLD_THREAD_LOCK_SCOPE;

    s_messageByThread[GetThreadID()] = std::string(msg);
}

void gu::MemLeakDetector::ResetProfileMessage()
{
    gu::MemLeakDetector::SetProfileMessage("");
}

void gu::MemLeakDetector::LogFree(const void* ptr, int size, int line, string& file)
{
	*s_log << "FREE: " << ptr << " | Size: " << size << " | file: " << file << "(" << line << ")" << endl;
	s_log->flush();
}
void gu::MemLeakDetector::LogAlloc(const void* ptr, int size, int line, string& file)
{
	*s_log << "ALLOC: " << ptr << " | Size: " << size << " | file: " << file << "(" << line << ")" << endl;
	s_log->flush();
}

void gu::MemLeakDetector::LogError(string& message)
{
    MLD_THREAD_LOCK_SCOPE;

	*s_log << "ERROR: " << message << endl;
	s_log->flush();
}


void gu::MemLeakDetector::Enable(bool enable)
{
    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

	MLD_THREAD_LOCK_SCOPE;

    s_enableByThread[GetThreadID()] = enable;
}


void gu::MemLeakDetector::PrintStatus() 
{


    if(!m_initialized) 
    {
        LogError(string("MemLeakDetector::Initialize() was not called!"));
        return;
    }

	MLD_THREAD_LOCK_SCOPE;

    if(s_leaksFilename.size() != 0)
    {   
        // Open the log file for output
        ofstream out( s_leaksFilename.c_str() );
        
        if(out.is_open())
        {        
            out << "Total Number of Pointers: " << s_memoryMap.size() << endl;
            out << "Total Size Allocated: " << m_totalSize << endl;
            
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


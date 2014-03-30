

#include "gu/Threads.h"


namespace gu 
{
    ThreadID GetThreadID()
    {

    #if USE_PTHREAD
	    #ifdef WIN32
        ThreadID tid = (ThreadID)((void*)(pthread_self().p));
	    #else
        ThreadID tid = pthread_self();
	    #endif
    #else
	    ThreadID tid = std::this_thread::get_id();
    #endif

        return tid;
    }
} //namespace gu



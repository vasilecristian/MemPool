#ifndef Threads_h
#define Threads_h


#include <thread>
#include <mutex>


typedef std::thread::id ThreadID;

namespace gu
{
    /** @return the thread ID */
    ThreadID GetThreadID();
}

#endif //Threads_h
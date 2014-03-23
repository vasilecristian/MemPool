#ifndef Threads_h
#define Threads_h


#include <thread>
#include <mutex>


typedef std::thread::id ThreadID;

#define THREAD_MUTEX_LOCK_SCOPE(mutex_reference)  std::lock_guard<std::mutex> lock(mutex_reference)
#define THREAD_MUTEX_RECURSIVE_LOCK_SCOPE(mutex_reference)  std::lock_guard<std::recursive_mutex> lock(mutex_reference)
#define THREAD_MUTEX_LOCK(mutex_reference) mutex_reference.lock()
#define THREAD_MUTEX_UNLOCK(mutex_reference) mutex_reference.unlock()



namespace gu
{
    /** @return the thread ID */
    ThreadID GetThreadID();
}

#endif //Threads_h
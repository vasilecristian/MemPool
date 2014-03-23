#include "log.h"
#include <iostream>
#include "Threads.h"

namespace gu
{
    ImplementSingleton(LogMgr);
    bool gu::LogMgr::m_initialized = false;
    std::recursive_mutex gu::LogMgr::s_mutexProtect;


    LogMgr::LogMgr()
    {

    }

    LogMgr::~LogMgr()
    {
    }



    void LogMgr::Initialize()
    {
        if(!m_initialized)
        {
            m_initialized = true;
        }
    
    }


    std::recursive_mutex& LogMgr::GetMutex()
    {
        if(!m_initialized)
            Initialize();

	    return s_mutexProtect;
    }



    #ifdef _WIN32
    #define COLOR_WHITE			FOREGROUND_BLUE|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
    #define COLOR_GRAY		    FOREGROUND_BLUE|FOREGROUND_GREEN|FOREGROUND_RED
    #define COLOR_RED			FOREGROUND_RED|FOREGROUND_INTENSITY
    #define COLOR_YELLOW		FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY
    #define COLOR_GREEN			FOREGROUND_GREEN|FOREGROUND_INTENSITY
    #define COLOR_BLUE			FOREGROUND_BLUE|FOREGROUND_INTENSITY
    #define COLOR_BLUE_LIGHT    FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY
    #define COLOR_YELLOW_DARK   FOREGROUND_RED|FOREGROUND_GREEN

    void LogMgr::ChangeColour(WORD theColour)
    {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);  // Get handle to standard output

        SetConsoleTextAttribute(hConsole,theColour);  // set the text attribute of the previous handle
    }
    #else
    void LogMgr::ChangeColour(WORD theColour)
    {
    
    }
    #endif //_WIN32


    std::string LogMgr::Log(int level, const char* msg)
    {
        THREAD_MUTEX_RECURSIVE_LOCK_SCOPE(gu::LogMgr::GetMutex());

	    ThreadID pid = GetThreadID();

        std::string lvlMsg = "info";

        if(level == LOG_INFO)
        {
            ChangeColour(COLOR_WHITE);
        }
        else if(level == LOG_DEBUG)   
        {
            ChangeColour(COLOR_YELLOW_DARK);
            lvlMsg = "debug";
        }
        else if(level == LOG_WARNING)
        {
            ChangeColour(COLOR_YELLOW);
            lvlMsg = "warning";
        }
        else if(level == LOG_ERROR)   
        {
            ChangeColour(COLOR_RED);
            lvlMsg = "error";
        }
        else if(level == LOG_FATAL)
        {
            ChangeColour(COLOR_RED);
            lvlMsg = "fatal error";
        }

	    std::stringstream ss;
	    ss << pid << " " << lvlMsg << ": " << msg << endl;
	    std::string lastMsg = ss.str();

	    std::cout << lastMsg;
	

        ChangeColour(COLOR_WHITE);

	    return lastMsg;
    }

    std::string LogMgr::Log(int level, const std::stringstream& msg)
    {
        return Log(level, msg.str().c_str());
    }

    std::string LogMgr::LogEX(int level, const char* fileName, int lineNumber, const char* msg)
    {
        std::stringstream ss;
        ss << fileName << "(" << lineNumber << "): " << msg;
        return Log(level, ss.str().c_str());
    }

    std::string LogMgr::LogEX(int level, const char* fileName, int lineNumber, const std::stringstream& msg)
    {   
        std::stringstream ss;
        ss << fileName << "(" << lineNumber << "): " << msg.str();
        return Log(level, ss.str().c_str());
    }


} //namespace gu
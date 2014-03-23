#ifndef LOG_H
#define LOG_H

// standart include 
#include <sstream> 
#include <mutex>

#ifdef _WIN32
#include <windows.h>
#endif //_WIN32



// specific include 
#include "Singleton.hpp"



namespace gu
{
    /** Singleton used to log messages. Is thread safe. */
    class LogMgr
    {
        /** Declare as singleton */
        DeclareSingleton(LogMgr);

        /** the default constructor */
        LogMgr();

        /** the destructor */
        ~LogMgr();

		
        /** flag that specify if the class was initialized or not. */ 
        static bool m_initialized;

        /** this is used to initialize the class */
        static void Initialize();

        /** protect the write with this mutex */
        static std::recursive_mutex s_mutexProtect;

        /**
         * We need to protect the map with this mutex. 
         * If this will be removed may occur some crashes when multiple threads will use the new and/or delete!
		 * This function is an workaround , because the static mutex is not created otherwise! probably this is a BUG!
         */
		static std::recursive_mutex& GetMutex();
	
		/**
         * Used to change the color of the next displayed text in console.
         * @param theColour is the color (values: FOREGROUND_BLUE, FOREGROUND_RED, FOREGROUND_WHITE ...)
         */
        void ChangeColour(WORD theColour);

    public:

        /** debug levels */
        enum LOG_LEVEL
        {
            LOG_INFO = 0,
            LOG_DEBUG,
            LOG_WARNING,
            LOG_ERROR,
            LOG_FATAL,
        };

        /** used to log a message
         * @param level takes a value from the LOG_LEVEL enum.
         * @param msg is the message to be shown.
         * @return the formatted message
         */
        std::string Log(int level, const char* msg);

        /** used to log a message
         * @param level takes a value from the LOG_LEVEL enum.
         * @param msg is the message to be shown.
         * @return the formatted message
         */
        std::string Log(int level, const std::stringstream& msg);

        /** used to log a message
         * @param level takes a value from the LOG_LEVEL enum.
         * @param fileName is the file name of the source file where this message occures.
         * @param lineNumber the line number in the fileName.
         * @param msg is the message to be shown.
         * @return the formatted message
         */
        std::string LogEX(int level, const char* fileName, int lineNumber, const char* msg);

        /** used to log a message
         * @param level takes a value from the LOG_LEVEL enum.
         * @param fileName is the file name of the source file where this message occures.
         * @param lineNumber the line number in the fileName.
         * @param msg is the message to be shown.
         * @return the formatted message
         */
        std::string LogEX(int level, const char* fileName, int lineNumber, const std::stringstream& msg);

    };

}; //namespace gu

#endif //LOG_H
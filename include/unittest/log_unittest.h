// Copyright 2013, Cristian Vasile
// All rights reserved.
#ifndef log_unittest_h
#define log_unittest_h

#include "log.h"
#include "gtest/gtest.h"

#include <mutex>
#include <thread>

TEST(Log, ConsoleVisualTest)
{
	std::string input = "ConsoleVisualTest -> LOG_INFO -> white color!";
	std::string result = vsge::LogMgr::GetInstance()->Log(vsge::LogMgr::LOG_INFO, input.c_str());
	std::size_t found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_DEBUG -> yellow dark color!";
	result = vsge::LogMgr::GetInstance()->Log(vsge::LogMgr::LOG_DEBUG, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_WARNING -> yellow color!";
	result = vsge::LogMgr::GetInstance()->Log(vsge::LogMgr::LOG_WARNING, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_ERROR -> red color!";
	result = vsge::LogMgr::GetInstance()->Log(vsge::LogMgr::LOG_ERROR, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_FATAL -> red color!";
	result = vsge::LogMgr::GetInstance()->Log(vsge::LogMgr::LOG_FATAL, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);
}


TEST(LogEX, ConsoleVisualTest)
{
	std::string input = "ConsoleVisualTest -> LOG_INFO -> white color!";
	std::string result = vsge::LogMgr::GetInstance()->LogEX(vsge::LogMgr::LOG_INFO, __FILE__, __LINE__, input.c_str());
	std::size_t found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_DEBUG -> yellow dark color!";
	result = vsge::LogMgr::GetInstance()->LogEX(vsge::LogMgr::LOG_DEBUG, __FILE__, __LINE__, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_WARNING -> yellow color!";
	result = vsge::LogMgr::GetInstance()->LogEX(vsge::LogMgr::LOG_WARNING, __FILE__, __LINE__, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_ERROR -> red color!";
	result = vsge::LogMgr::GetInstance()->LogEX(vsge::LogMgr::LOG_ERROR, __FILE__, __LINE__, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

	input = "ConsoleVisualTest -> LOG_FATAL -> red color!";
	result = vsge::LogMgr::GetInstance()->LogEX(vsge::LogMgr::LOG_FATAL, __FILE__, __LINE__, input.c_str());
	found = result.find(input);
	ASSERT_NE(found, std::string::npos);

}

static std::mutex s_protectEndFlag;
static unsigned int s_runningThreads = 0;
void ThreadFunction(vsge::LogMgr::LOG_LEVEL logLevel)
{
	s_protectEndFlag.lock();
	s_runningThreads++;
	s_protectEndFlag.unlock();

	for(int i=0; i < 3; i++)
	{
		std::stringstream ss;
		ss << i;
		vsge::LogMgr::GetInstance()->Log(logLevel, ss);
	}

	s_protectEndFlag.lock();
	s_runningThreads--;
	s_protectEndFlag.unlock();
}


TEST(LogDeathTest, MultiThread_Safe_Test)
{
	std::thread t1(std::bind(ThreadFunction, vsge::LogMgr::LOG_INFO));
	t1.detach();
	std::thread t2(std::bind(ThreadFunction, vsge::LogMgr::LOG_DEBUG));
	t2.detach();
	std::thread t3(std::bind(ThreadFunction, vsge::LogMgr::LOG_WARNING));
	t3.detach();
	std::thread t4(std::bind(ThreadFunction, vsge::LogMgr::LOG_ERROR));
	t4.detach();
	std::thread t5(std::bind(ThreadFunction, vsge::LogMgr::LOG_FATAL));
	t5.detach();

	while(s_runningThreads);

	ASSERT_TRUE(true);
}


#endif //log_unittest_h
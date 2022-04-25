// MemPoolTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <chrono>
#include <memory>
#include "MemPool/MemPool.h"

#define TEST_INIT(nameOfTest)	\
	std::string testName = nameOfTest;	\
	int testReturnValue = 0;

#define TEST_RETURN_VALUE testReturnValue

#define CHECK(condition) \
	if(!(condition)) { std::cout << "Failed: " << testName << " (" << __FILE__ << ":" << __LINE__ << ")" << std::endl; testReturnValue++; }	\
	else { std::cout << "Passed: " << testName << std::endl; }

int Test1()
{
	TEST_INIT("Test1!");
	
	// Arrange
	
	
	// Act
	
	
	// Assert
	//CHECK(value == valueExpected);
	
	
	return TEST_RETURN_VALUE;
}

using namespace std;

class A : public mp::BaseMemPool
{
public:
	int m_a;
	int m_b;
	int m_c;
	int m_d;
};



class B
{
public:
	int m_a;
	int m_b;
	int m_c;
	int m_d;
};

int main(int argc, const char* argv[])
{
	int size = 100000000;
	mp::MemPoolMgr::InitPool(sizeof(A), size);

	unsigned long start = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


	for (int i = 0; i < size; i++)
	{
		A* a = new A();
		a->m_a = 10;
		a->m_b = 20;
		a->m_c = 30;
		a->m_d = 40;
		delete a;
	}

	unsigned long end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "pool elapsed time: " << end - start << endl;
	start = end;


	for (int i = 0; i < size; i++)
	{
		B* a = new B();
		a->m_a = 10;
		a->m_b = 20;
		a->m_c = 30;
		a->m_d = 40;
		delete a;
	}

	end = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "no pool elapsed time: " << end - start << endl;
	start = end;


	//cout << a.m_a << " " << a.m_b << " " << a.m_c << " " << a.m_d << " " << endl;

	mp::MemPoolMgr::DeInitPool();
	return 0;
}


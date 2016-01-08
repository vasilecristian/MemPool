// MemPoolTest.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <iostream>
#include <chrono>
#include "MemPool/MemPool.h"

using namespace std;

class A : public mp::IMemPool
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
	int size = 1000000;
	mp::MemPool::InitPool(sizeof(A), size);

	unsigned long micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();


	for (int i = 0; i < size; i++)
	{
		A* a = new A();
		a->m_a = 10;
		a->m_b = 20;
		a->m_c = 30;
		a->m_d = 40;
		delete a;
	}

	unsigned long micro2 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "pool elapsed time: " << micro2 - micro << endl;

	for (int i = 0; i < size; i++)
	{
		B* a = new B();
		a->m_a = 10;
		a->m_b = 20;
		a->m_c = 30;
		a->m_d = 40;
		delete a;
	}

	unsigned long micro3 = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	cout << "no pool elapsed time: " << micro3 - micro2 << endl;

	//cout << a.m_a << " " << a.m_b << " " << a.m_c << " " << a.m_d << " " << endl;

	mp::MemPool::DeInitPool();
	return 0;
}


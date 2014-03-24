// Copyright 2013, Cristian Vasile
// All rights reserved.
#ifndef SmartP_unittest_h
#define SmartP_unittest_h

#include "smart_ptr.hpp"
#include "gtest/gtest.h"

#include "vsge/utils/log.h"

#include <thread>
#include <mutex>

using namespace vsge;


#define CICLES_PER_THREAD 10000
#define THREADS_NUM 50


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
TEST(smart_ptr, BasicTest)
{
	int *a = new int(1);
	smart_ptr<int> sp1(a);
	smart_ptr<int> sp2 = sp1;
	smart_ptr<int> sp3 = sp2;
	smart_ptr<int> sp4 = sp3;

	ASSERT_EQ(*sp1, 1);
	ASSERT_EQ(*sp2, 1);
	ASSERT_EQ(*sp3, 1);
	ASSERT_EQ(*sp4, 1);
	ASSERT_EQ(sp1.GetRefCount(), 4);

	for(int i=0; i<100; i++)
	{
		smart_ptr<int> sp5 = sp4;
		(*sp5)++; 
	}

	ASSERT_EQ(*sp1, 101);
	ASSERT_EQ(sp1.GetRefCount(), 4);
}
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static std::mutex s_SmartPUnitTest_protectEndFlag;
static unsigned int s_SmartPUnitTest_increment = 0;
static unsigned int s_SmartPUnitTest_sp = 0;
static unsigned int s_SmartPUnitTest_runningThreads = 0;
static unsigned int s_SmartPUnitTest_startedThreads = 0;
static unsigned int s_SmartPUnitTest_stoppedThreads = 0;
void smart_ptr_UnitTest_ThreadFunction1(smart_ptr<int> sp)
{
	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_startedThreads++;
	s_SmartPUnitTest_runningThreads++;
	
	s_SmartPUnitTest_protectEndFlag.unlock();

	for(int i=0; i < CICLES_PER_THREAD; i++)
	{
		//use the operator =
		smart_ptr<int> sp_ = sp;

		//use the operator*
		int& a = (*sp_); 

		s_SmartPUnitTest_protectEndFlag.lock();
		a++;
		s_SmartPUnitTest_protectEndFlag.unlock();

		//use the operator ==
		if(sp_ == sp) {}

		//use the operator !=
		if(sp_ != sp) {}



		s_SmartPUnitTest_protectEndFlag.lock();
		s_SmartPUnitTest_increment++;
		if(s_SmartPUnitTest_sp == 0) s_SmartPUnitTest_sp = (*sp_);
		else s_SmartPUnitTest_sp++;
		s_SmartPUnitTest_protectEndFlag.unlock();
	}

	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_stoppedThreads++;
	s_SmartPUnitTest_runningThreads--;
	s_SmartPUnitTest_protectEndFlag.unlock();
}


TEST(smart_ptr_DeathTest, MultiThread_Safe_Test1)
{
	smart_ptr<int> sp(new int(0));

	ASSERT_EQ(*sp, 0);

	for(int i=0; i < THREADS_NUM; i++)
	{
		std::thread t(smart_ptr_UnitTest_ThreadFunction1, sp);
		t.detach();
	}

    while(s_SmartPUnitTest_runningThreads){};
	
	ASSERT_EQ(s_SmartPUnitTest_increment, THREADS_NUM * CICLES_PER_THREAD);
	ASSERT_EQ(s_SmartPUnitTest_sp, THREADS_NUM * CICLES_PER_THREAD);
	ASSERT_EQ(s_SmartPUnitTest_startedThreads, THREADS_NUM);
	ASSERT_EQ(s_SmartPUnitTest_stoppedThreads, THREADS_NUM);
	ASSERT_EQ(sp.GetRefCount(), 1);
	ASSERT_EQ(*sp, THREADS_NUM * CICLES_PER_THREAD );
}
//-----------------------------------------------------------------------------



//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TestClassA : public IRTTI<TestClassA>
{
public:
	int count;
	int a;
	TestClassA() { a = 0; count = 0;};
	virtual int GetValue(){ return a; };
};

class TestClassB : public IRTTI<TestClassB>
				 , virtual public TestClassA
{
public:
	int b;
	TestClassB()
	{
		b = 1; 
	};
	virtual int GetValue() override { return b; } ;
};

class TestClassC : public IRTTI<TestClassC>
				 , public TestClassB
{
public:
	int c;
	TestClassC()
	{
		c = 2; 
	};
	virtual int GetValue() override { return c; } ;
};


class TestClassD : public IRTTI<TestClassD>
				 , virtual public TestClassA
				 , public TestClassC
				 
{
public:
	int d;
	int* e;
	TestClassD()
	{
		d = 3; 
		e = new int(4);
	};

	~TestClassD()
	{
		delete e;
	};

	int GetValue() override { return d; } ;
};

void smart_ptr_UnitTest_ThreadFunction2(smart_ptr<TestClassD> sp)
{
	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_startedThreads++;
	s_SmartPUnitTest_runningThreads++;
	s_SmartPUnitTest_protectEndFlag.unlock();

	for(int i=0; i < CICLES_PER_THREAD; i++)
	{
		//use the operator=
		smart_ptr<TestClassD> sp_local = sp;

		//use the operator==
		if(sp_local == sp) {}

		//use the operator!=
		if(sp_local != sp) {}

		smart_ptr<TestClassC> spC = sp_local.DynamicCast<TestClassC>();
        
        smart_ptr<TestClassB> spB = spC.DynamicCast<TestClassB>();

        smart_ptr<TestClassA> spA = spB.DynamicCast<TestClassA>();

		//use the operator* and operator=
		TestClassA& obj = (*spA); 

		s_SmartPUnitTest_protectEndFlag.lock();
		obj.count++;

		s_SmartPUnitTest_increment++;

		if(s_SmartPUnitTest_sp == 0) 
            s_SmartPUnitTest_sp = obj.count;
		else 
            s_SmartPUnitTest_sp++;

		s_SmartPUnitTest_protectEndFlag.unlock();
	}

	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_stoppedThreads++;
	s_SmartPUnitTest_runningThreads--;
	s_SmartPUnitTest_protectEndFlag.unlock();
};



TEST(smart_ptr_DeathTest, MultiThread_Safe_Test2)
{
	s_SmartPUnitTest_increment = 0;
	s_SmartPUnitTest_sp = 0;
	s_SmartPUnitTest_runningThreads = 0;
	s_SmartPUnitTest_startedThreads = 0;
	s_SmartPUnitTest_stoppedThreads = 0;

	smart_ptr<TestClassD> sp(new TestClassD());

	ASSERT_EQ(sp->count, 0);

	for(int i=0; i < THREADS_NUM; i++)
	{
		std::thread t(smart_ptr_UnitTest_ThreadFunction2, sp);
		t.detach();
	}

	while(s_SmartPUnitTest_runningThreads){};
	
	ASSERT_EQ(s_SmartPUnitTest_increment, THREADS_NUM * CICLES_PER_THREAD);
    ASSERT_EQ(s_SmartPUnitTest_sp, THREADS_NUM * CICLES_PER_THREAD);
	ASSERT_EQ(s_SmartPUnitTest_startedThreads, THREADS_NUM);
	ASSERT_EQ(s_SmartPUnitTest_stoppedThreads, THREADS_NUM);
	ASSERT_EQ(sp.GetRefCount(), 1);
	ASSERT_EQ(sp->count, THREADS_NUM * CICLES_PER_THREAD);
};
//-----------------------------------------------------------------------------







//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
class TestClassA2 : public IRTTI<TestClassA2>
{
public:
	int count;
	int a;
	TestClassA2() { a = 0; count = 0;};
	virtual int GetValue(){ return a; };
};

class TestClassB2 : public IRTTI<TestClassB2>
				  , virtual public TestClassA2
{
public:
	int b;
	TestClassB2()
	{
		b = 1; 
	};
	virtual int GetValue() override { return b; } ;
};

class TestClassC2 : public IRTTI<TestClassC2>
				  , public TestClassB2
{
public:
	int c;
	TestClassC2()
	{
		c = 2; 
	};
	virtual int GetValue() override { return c; } ;
};


class TestClassD2 : public IRTTI<TestClassD2>
				 , virtual public TestClassA2
				 , public TestClassC2
				 
{
public:
	int d;
	int* e;
	TestClassD2()
	{
		d = 3; 
		e = new int(4);
	};

	~TestClassD2()
	{
		delete e;
	};

	int GetValue() override { return d; } ;
};

void smart_ptr_UnitTest_ThreadFunction3(smart_ptr<TestClassD2> sp)
{
	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_startedThreads++;
	s_SmartPUnitTest_runningThreads++;
	s_SmartPUnitTest_protectEndFlag.unlock();

	for(int i=0; i < CICLES_PER_THREAD; i++)
	{
		//use the operator=
		smart_ptr<TestClassD2> sp_local = sp;

		//use the operator==
		if(sp_local == sp) {}

		//use the operator!=
		if(sp_local != sp) {}

		smart_ptr<TestClassC2> spC = sp_local.DynamicCast<TestClassC2>();
        
        smart_ptr<TestClassB2> spB = spC.DynamicCast<TestClassB2>();

        smart_ptr<TestClassA2> spA = spB.DynamicCast<TestClassA2>();

		//use the operator* and operator=
		TestClassA2& obj = (*spA); 

		s_SmartPUnitTest_protectEndFlag.lock();
		obj.count++;

		s_SmartPUnitTest_increment++;

		if(s_SmartPUnitTest_sp == 0) 
            s_SmartPUnitTest_sp = obj.count;
		else 
            s_SmartPUnitTest_sp++;

		s_SmartPUnitTest_protectEndFlag.unlock();
	}

	s_SmartPUnitTest_protectEndFlag.lock();
	s_SmartPUnitTest_stoppedThreads++;
	s_SmartPUnitTest_runningThreads--;
	s_SmartPUnitTest_protectEndFlag.unlock();
};



TEST(smart_ptr_DeathTest, MultiThread_Safe_Test3)
{
	s_SmartPUnitTest_increment = 0;
	s_SmartPUnitTest_sp = 0;
	s_SmartPUnitTest_runningThreads = 0;
	s_SmartPUnitTest_startedThreads = 0;
	s_SmartPUnitTest_stoppedThreads = 0;

    TestClassD2* spp = new TestClassD2();

    //smart_ptr<TestClassD2> test = spp->shared_from_this();

	smart_ptr<TestClassD2> sp(spp);

	ASSERT_EQ(sp->count, 0);

	for(int i=0; i < THREADS_NUM; i++)
	{
        std::thread t(smart_ptr_UnitTest_ThreadFunction3, sp);
		t.detach();
	}

	while(s_SmartPUnitTest_runningThreads){};
	
	ASSERT_EQ(s_SmartPUnitTest_increment, THREADS_NUM * CICLES_PER_THREAD);
    ASSERT_EQ(s_SmartPUnitTest_sp, THREADS_NUM * CICLES_PER_THREAD);
	ASSERT_EQ(s_SmartPUnitTest_startedThreads, THREADS_NUM);
	ASSERT_EQ(s_SmartPUnitTest_stoppedThreads, THREADS_NUM);
	ASSERT_EQ(sp.GetRefCount(), 1);
	ASSERT_EQ(sp->count, THREADS_NUM * CICLES_PER_THREAD);
};
//-----------------------------------------------------------------------------



#endif //log_unittest_h
/************************************************************************/
/*File created on 16.09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef SINGLETON_H
#define SINGLETON_H

#include "Utils.h"


/////////////////////////////////////////////////////////////
/**
 * Standard declaration for a singletone 
 */
#define DeclareSingleton(type)                              \
public:                                                     \
	inline static void CreateInstance()						\
	{														\
		GU_ASSERT( type::s_pInstance == NULL);            \
        if(type::s_pInstance == NULL)                       \
        {                                                   \
            type::s_pInstance = new type();                 \
        }                                                   \
	}                                                       \
	inline static type * GetInstance()                      \
	{                                                       \
		if(type::s_pInstance == NULL)                       \
        {                                                   \
            type::s_pInstance = new type();                 \
        }													\
        return type::s_pInstance;                           \
	}                                                       \
	static void DeleteInstance()                            \
	{                                                       \
		if(type::s_pInstance) delete type::s_pInstance;     \
		type::s_pInstance = 0;                              \
	}                                                       \
private:                                                    \
	static type * s_pInstance;                              \
    type( const type &/*obj*/ ){}                           \
    type & operator=(const type &/*rhs*/){ return *this;}   \



/**
 * Standard declaration for a singletone 
 */
#define ImplementSingleton(type)                            \
	type * type::s_pInstance = 0;                           \

/////////////////////////////////////////////////////////////





/////////////////////////////////////////////////////////////
/**
 * This macro will add the Singleton specific code to the Driver base class.
 * The special thing regarding this Singleton is that does not create and instance.
 * The GetInstance() will not create the instance if does not exists, and will throw 
 * an assertion (on debug config).
 * The instance must be created only by using the CreateInstance() function from 
 * derived classes. So, you cannot use this class without a derived class.
 * Use this macro only in Driver class (aka base Driver class)
 */
#define DeclareSingleton_BaseDriver(type)                   \
public:                                                     \
    inline static type * GetInstance()                      \
    {                                                       \
        if(type::s_pInstance == NULL)                       \
        {                                                   \
            GU_ASSERT(type::s_pInstance != NULL);         \
        }                                                   \
        return type::s_pInstance;                           \
    }                                                       \
    static void DeleteInstance()                            \
    {                                                       \
        if(type::s_pInstance) delete type::s_pInstance;     \
        type::s_pInstance = 0;                              \
    }                                                       \
    type( const type &obj ){}                               \
    type & operator=(const type &rhs){ return *this;}       \
protected:                                                  \
    static type * s_pInstance;                              \
    


/**
 * This macro will initialize the static member of the singleton.
 * This is used only in Driver class (the base class).
 * Do not use it on other classes.
 */
#define ImplementSingleton_BaseDriver(type)                 \
    type * type::s_pInstance = 0;                           \


/**
 * This must be used only in Driver derived class.
 * This will add the CreateInstance static function that will create 
 * an instance of this class. As soon the CreateInstance function is 
 * called, the Driver::GetInstance() can be used.
 */
#define DeclareSingleton_DerivedDriver(type, BaseType)      \
public:                                                     \
    inline static BaseType * CreateInstance()               \
    {                                                       \
        if(s_pInstance == NULL)                             \
        {                                                   \
            s_pInstance = new type();                       \
        }                                                   \
        return s_pInstance;                                 \
    }                                                       \
    type( const type &obj ){}                               \
    type & operator=(const type &rhs){ return *this;}       \

/////////////////////////////////////////////////////////////



#endif //SINGLETON_H

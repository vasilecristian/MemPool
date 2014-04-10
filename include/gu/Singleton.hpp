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



#endif //SINGLETON_H

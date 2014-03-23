/************************************************************************/
/*File created on 03.2013 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef SMARTP_BASE_H
#define SMARTP_BASE_H

#include <RTTI.h>

#include <mutex>
#include <memory>

#define USE_THREAD_SAFE_SMARTP 1

namespace fastrtti
{

    /** This is the template class implementation of a smart pointer. */
    template < typename T >
    class smart_ptr
    {

    private:

        /** protect the data. */
		#if USE_THREAD_SAFE_SMARTP
		std::recursive_mutex* m_mutexProtection;
		#endif //USE_THREAD_SAFE_SMARTP

        /** Generic pointer to be stored */
        T* m_pData;

        /**
         * This is the reference counter. 
         * Will be incremented for each reference of this pointer.
         */
        long* m_referenceCounter;

    public:

        /** The default constructor, initialize the reference counter with value 1. */
        smart_ptr()
        {	
            #if USE_THREAD_SAFE_SMARTP
			m_mutexProtection = new std::recursive_mutex;
			#endif //USE_THREAD_SAFE_SMARTP

			m_pData = nullptr;
			m_referenceCounter = new long(1);
        }

        /** The constructor, initialize the reference counter with value 1. */
        smart_ptr(T* pValue)
        {
            #if USE_THREAD_SAFE_SMARTP
			m_mutexProtection = new std::recursive_mutex;
			#endif //USE_THREAD_SAFE_SMARTP

			m_pData = pValue;
			m_referenceCounter = new long(1);
        }


        /**
         * The constructor, that receive also the reference counter.
         * This is used to copy one smart_ptr to another smart_ptr. 
         * The new smart_ptr is like a reference to another smart_ptr.
         */
		#if USE_THREAD_SAFE_SMARTP
        smart_ptr(T* pValue, long* referenceCounter, std::recursive_mutex* mp)
		#else //!USE_THREAD_SAFE_SMARTP
		smart_ptr(T* pValue, long* referenceCounter)
		#endif //USE_THREAD_SAFE_SMARTP
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*mp);
			m_mutexProtection = mp;
			#endif //USE_THREAD_SAFE_SMARTP


			m_pData = pValue;
			m_referenceCounter = referenceCounter;
            (*m_referenceCounter)++;
        }


        /** Copy constructor, increment the reference counter. */
        smart_ptr(const smart_ptr<T>& sp) 
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*(sp.m_mutexProtection));
			#endif //USE_THREAD_SAFE_SMARTP
			

			m_pData = sp.m_pData;
			m_referenceCounter = sp.m_referenceCounter;
            (*m_referenceCounter)++;

			#if USE_THREAD_SAFE_SMARTP
			m_mutexProtection = sp.m_mutexProtection;
			#endif //USE_THREAD_SAFE_SMARTP
        }

		/** This will return the actual value of reference counter
         * @return the reference counter
         */
		inline int GetRefCount()
		{
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

			return *m_referenceCounter;
		}

        /**
         * The destructor will decrement the reference counter and destroy the 
         * pointer if reference counter is 0.
         */
        virtual ~smart_ptr()
        {
			#if USE_THREAD_SAFE_SMARTP
			m_mutexProtection->lock();
			#endif //USE_THREAD_SAFE_SMARTP

            (*m_referenceCounter)--;
            if((*m_referenceCounter) == 0)
            {
                delete m_pData;
                m_pData = nullptr;
                delete m_referenceCounter;
                m_referenceCounter = nullptr;

				#if USE_THREAD_SAFE_SMARTP
				m_mutexProtection->unlock();
				delete m_mutexProtection;
                m_mutexProtection = nullptr;
				return;
				#endif //USE_THREAD_SAFE_SMARTP
            }

			#if USE_THREAD_SAFE_SMARTP
			m_mutexProtection->unlock();
			#endif //USE_THREAD_SAFE_SMARTP
        }

        /** Assignment operator */
        smart_ptr<T>& operator = (const smart_ptr<T>& sp)
        {
            if (this != &sp) // Avoid self assignment
            {
				#if USE_THREAD_SAFE_SMARTP
				m_mutexProtection->lock();
				#endif //USE_THREAD_SAFE_SMARTP

                (*m_referenceCounter)--;
                if((*m_referenceCounter) == 0)
                {
                    delete m_pData;
                    delete m_referenceCounter;

					#if USE_THREAD_SAFE_SMARTP
					m_mutexProtection->unlock();
					delete m_mutexProtection;
					m_mutexProtection = nullptr;
					#endif //USE_THREAD_SAFE_SMARTP
                }

				#if USE_THREAD_SAFE_SMARTP
				std::lock_guard<std::recursive_mutex> lock(*sp.m_mutexProtection);
				#endif //USE_THREAD_SAFE_SMARTP

                // Copy the data and reference pointer
                // and increment the reference count
                m_pData = sp.m_pData;
                m_referenceCounter = sp.m_referenceCounter;
                (*m_referenceCounter)++;

				#if USE_THREAD_SAFE_SMARTP
				std::recursive_mutex* saveTheMutex = m_mutexProtection;
				m_mutexProtection = sp.m_mutexProtection;

				if(saveTheMutex)
					saveTheMutex->unlock();
				#endif //USE_THREAD_SAFE_SMARTP
            }
            return *this;
        }

        /**
         * 'Equal to' operator
         * @param sp is the right operator.
         * @return true if m_pData == sp.m_pData and m_referenceCounter == sp.m_referenceCounter
         */
        inline bool operator==(const smart_ptr<T>& sp) 
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return ((m_pData == sp.m_pData) && (m_referenceCounter == sp.m_referenceCounter));
        }

        /**
         * 'Not equal to' operator
         * @param sp is the right operator.
         * @return true if m_pData != sp.m_pData or m_referenceCounter != sp.m_referenceCounter
         */
        inline bool operator!=(const smart_ptr<T>& sp) 
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return ((m_pData != sp.m_pData) || (m_referenceCounter != sp.m_referenceCounter));
        }


        /**
         * Use this to cast the Smart pointer to another type of Smart pointer.
         * The result will be another type of smart_ptr , but with the same reference counter!
         * The referenceCounter will be increased in the constructor.
         * This is a template!
         * @return a smpart pointer of type smart_ptr< template argument >
         */
        template < typename G >
        inline smart_ptr<G> DynamicCast()
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            G* ptr = fastrtti_dynamic_cast<G>(m_pData);
            VSGE_ASSERT(ptr != NULL);
            if(ptr)
            {
			    #if USE_THREAD_SAFE_SMARTP
			    smart_ptr<G> sp(ptr, m_referenceCounter, m_mutexProtection);            
			    #else //!USE_THREAD_SAFE_SMARTP
			    smart_ptr<G> sp(ptr, m_referenceCounter);            
			    #endif //USE_THREAD_SAFE_SMARTP

                return sp;
            }
            else
            {
                smart_ptr<G> sp(NULL); 
                return sp;
            }
            
        }


        /** Specialized Cast that will return smart_ptr<RTTI> */
        template <>
        inline smart_ptr<RTTI> DynamicCast<RTTI>()
        {   
			//this assert is here to generate an compile error !!!!!!
			//if the object from pointer m_pData does not extent the IRTTI interface 
            //a compile error is generated! 
			VSGE_ASSERT(m_pData->IsRTTI());

			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			return smart_ptr<RTTI>((RTTI*)m_pData, m_referenceCounter, m_mutexProtection);
			#else //!USE_THREAD_SAFE_SMARTP
			return smart_ptr<RTTI>((RTTI*)m_pData, m_referenceCounter);
			#endif //USE_THREAD_SAFE_SMARTP
        }

        /**
         * Specialized Cast that will return smart_ptr<RTTI>
         */
        inline smart_ptr<RTTI> DynamicCastToSmartRTTI()
        {
            return DynamicCast<RTTI>();
        }

		/**
         * @return true if the data is NOT NULL (not nullptr)
         */
        inline bool IsValid()
        {
            return (m_pData != nullptr)?true:false;
        }

        /** @return the reference to the object. */
        inline T& operator* ()
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return *m_pData;
        }

        /** @return the real pointer. */
        inline T* operator-> ()
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return m_pData;
        }

        /** @return the real pointer. */
        inline T* GetPtr()
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return m_pData;
        }

        /** Used to get the pointer casted to anothed type. For casting is used
         * the fast-rtti mechanism.
         * @return the real pointer casted to another type. 
         */
        template < typename U >
        inline U* GetPtr()
        {
			#if USE_THREAD_SAFE_SMARTP
			std::lock_guard<std::recursive_mutex> lock(*m_mutexProtection);
			#endif //USE_THREAD_SAFE_SMARTP

            return fastrtti_dynamic_cast<U>(m_pData);
        };

    };



};



#endif //SMARTP_BASE_H
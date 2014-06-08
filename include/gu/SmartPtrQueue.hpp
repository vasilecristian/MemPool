/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef SMARTPQUEUE_H
#define SMARTPQUEUE_H

#include "Singleton.hpp"
#include "SmartPtr.hpp"
#include <vector>

namespace gu
{

    template<class O>
    class SmartPtrQueueBase
    {
    protected:

        /**
         * The default constructor.
         */
        SmartPtrQueueBase()
        {
        }

        /**
         * The default destructor.
         */
        virtual ~SmartPtrQueueBase()
        {
        }
       

    public:
        /**
         * return the number of elements
         */
        virtual int GetSize() = 0;

        /**
         * Add an object to the list (push_back)
         * Must be implemented on derived class.
         * @param obj is a pointer of the object that must be added to the list. 
         */
        virtual void Add(SmartPtr<O> obj) = 0;

        /**
         * Add an object to the list (push_back)
         * Must be implemented on derived class.
         * @param obj is a pointer of the object that must be added to the list. 
         * @return the object just added as smart pointer, or a smart pointer to NULL.
         */
        virtual SmartPtr<O> Add(RTTI* obj) = 0;

        /**
         * Sort all objects using the function sent by parameter SortFunction.
         * @param SortFunction is a pointer to function. This function must compare the objects a and b
         *        and return a bool (depending of the result of compare).
         */
        virtual void SortAll(bool (*SortFunction)(SmartPtr<O> a, SmartPtr<O> b)) = 0;

        /**
         * Remove all objects from the list and delete them.
         * For each object will be called delete.
         */
        virtual void RemoveAll() = 0;

        
        /**
         * Search the object and remove it from the list. The destructor for this object 
         * will be called or not depends of delete_obj parameter.
         */
        virtual void Remove(SmartPtr<O> obj) = 0;

    };




    template<class O>
    class SmartPtrQueue : public SmartPtrQueueBase<O>
    {

    protected:

        std::vector< SmartPtr<O> > m_Q;

        /**
         * The default constructor.
         */
        SmartPtrQueue()
        {
        }

        /**
         * The default destructor.
         */
        virtual ~SmartPtrQueue()
        {
            m_Q.clear();
        }
       
    public:

        /**
         * @return an int representing the number of elements
         */
        virtual int GetSize() override
        {
            return m_Q.size();
        }


        /**
         * @param i is the index of the wanted object.
         * @return the element form position i                  
         */
        virtual inline SmartPtr<O> GetObj(int i)
        {
            return m_Q[i];
        }

        /**
         * Add an object to the list (push_back)
         * @param obj is a pointer of the object that must be added to the list. 
         */
        virtual void Add(SmartPtr<O> obj) override
        {
            m_Q.push_back(obj);
        }

        /**
         * Add an object to the list (push_back)
         *
         * @param obj is a pointer of the object that must be added to the list. 
         * @return the object just added as smart pointer, or a smart pointer to NULL.
         */
        virtual SmartPtr<O> Add(RTTI* obj) override
        {
            O* obj_ = (O*)dyna_cast<O>(obj);
            if(obj_ != NULL)    
            {
                SmartPtr<O> smartObj(obj_);
                Add(smartObj);

                return smartObj;
            }
            
            LogMgr::GetInstance()->LogEX(LogMgr::LOG_ERROR, __FILE__, __LINE__, "cannot add a null pointer to SmartPtrQueue!");
            return SmartPtr<O>(NULL);
        }


    protected:
        /**
         * sort all the objects using the function SortFunction.
         */
        virtual void SortAll(bool (*SortFunction)(SmartPtr<O> a, SmartPtr<O> b)) override
        {
            std::sort(m_Q.begin(), m_Q.end(), SortFunction);
        };

    public:
        /**
         * Remove all objects from the list.
         */
        virtual void RemoveAll() override
        {
            //this will remove the object from the list.
            m_Q.clear();  
        };

        


        /**
         * Search the object and remove it from the list. The destructor for this object 
         * will be called or not depends of delete_obj parameter.
         *
         * @param obj is a pointer to the object that must be removed (and deleted)
         * @param delete_obj is a bool that specify if the object are deleted, or only removed from the list.
         */
        virtual void Remove(SmartPtr<O> obj) override
        {
            int size = m_Q.size();
            for(int i=0; i < size; i++)
            {
                if(m_Q[i] == obj) 
                {
                    //the erase() will call the destructor of the object!
                    m_Q.erase(m_Q.begin()+i);

                    return;
                }
            }
        };

    };


} //namespace gu

#endif //SMARTPQUEUE_H

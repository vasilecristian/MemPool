/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef MANAGER_BASE_H
#define MANAGER_BASE_H


#include <RTTI.h>
#include "utils.h"

using namespace fastrtti;

namespace gu
{
    
    
    /**
     * This is the Base class for any Manager.
     * Is a template that receive as params a typename key and a class.
     * With this parameters a map will be created.
     */
    template<typename key, class managedClass>
    class ManagerBase
    {
    protected:

        /** this is the continer of this manager */
        std::map<key, managedClass> m_map;
        
    public:

        /** The constructor */
        ManagerBase(void)
        {
        }

        /** The Destructor. */
        virtual ~ManagerBase(void)
        {
            m_map.clear();
        }

        /**
         * Is the function where all the init code resides.
         * This is a virtual function and must be implemented by the derived classes.
         */
        virtual void Init(){};

        /**
         * All un-init code must be here.
         * This is a virtual function and must be implemented by the derived classes.
         */
        virtual void DeInit(){};


        /**
         * Detects what type of object is and add it to a special list.
         * This is a virtual function and must be implemented by the derived classes.
         */
        virtual void Add(key keyName, managedClass element)
        {
            m_map[keyName] = element;
        };

        /**
         * Detects what type of object is and remove it from the list. The destructor for this object is
         * called or not depending of delete_obj parameter.
         *
         * @param keyName is the key of the element in the map
         * @param delete_obj is a bool. If this is true the object will be deleted , not just removed from the map.
         */
        virtual void Remove(key keyName, bool delete_obj = false)
        {
            ///not implemented in this class (base class)
            GU_ASSERT(false);
        };

        /** Delete all elements from the map. */
        void DeleteAll(void)
        {
            ///not implemented in this class (base class)
            GU_ASSERT(false);
        }

        /**
         * Use this to retrieve one element from the map.
         *
         * @param keyName is the name of the key that you want get.
         * @return the element.
         */
        virtual managedClass Get(key keyName)
        {
            if(ExistInMap(keyName))
            {
                return m_map[keyName];
            }

            ///this means that the object must be already loaded, before to call Get!
            GU_ASSERT(false);
            return managedClass(NULL);
        }

		/**
         * Use this to check if element exist.
         *
         * @param keyName is the name of the key that you want get.
         * @return true if element exists.
         */
        inline bool ExistInMap(key keyName)
        {
            if(m_map.find(keyName) != m_map.end())
                return true;

            return false;
        }


        /**
         * Use this to Get the object from the map or load it if does 
         * not exists in the map. The object loaded is added to the map.
         * If the load fails will be returned an NULL object.
         * @param keyName is the name of the key that you want get.
         * @param file represent the filename to load.
         * @return the object loaded.
         */
        virtual managedClass GetOrLoadFromFile(const key& keyName, const std::string& file) = 0;
    };

} //namespace gu

#endif //MANAGER_BASE_H
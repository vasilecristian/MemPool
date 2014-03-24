/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef INSTANCE_FACTORY_H
#define INSTANCE_FACTORY_H

#include <map>
#include "Singleton.hpp"
#include "Params.h"
#include "log.h"
#include <fast-rtti/RTTI.h>
#include <sstream>


namespace gu
{

    /**
     * Used to declare/construct an object using the constructor that accept as parameter the ClassName.
     * We need this because the ClassName must be associated with the class.
     * If this macro isn't used the InstanceFactory class will don't know what type of instance to create 
     * for a specific ClassName.
     * This macro must be used outside of a class declaration.
     */
    #define REGISTER_TO_INSTANCE_FACTORY(className) static const className s_##className(#className);


    /**
     * Used to declare the needed constructors for a Factoryable class.
     * This must be used inside a class declaration.
     * The constructor "className(const std::string& typeID)" will be declared and implemented.
     *                 This is used in REGISTER_TO_INSTANCE_FACTORY to create the first instance of this class, 
     *                 that will set the class ID (aka className)
     * The constructor "className(Params* params)" . Used in CreateInstance static template function.
     *                 This is used to receive the pointer to structure that will contines all the values
     *                 needed by this class. The ideea is to load from a file (an xml with params) all the 
     *                 parameters and their values. This constructor will be used in CreateInstance function.
     * 
     */
    #define DECLARE_AS_FACTORYABLE(className) \
                                    public: \
                                        className(const std::string& typeID):Factoryable< className >(typeID){} \
                                        className(smart_ptr<Params> params):m_params(params){}; \
                                    private: \
                                        smart_ptr<Params> m_params; 



    /** This is a typedef for the function that can create the object. */
    typedef RTTI* (*FunctionThatCreatesInstance)(smart_ptr<Params>);


    /** This is a typedef for std::map<std::string, FunctionThatCreatesInstance> 
     * The REGISTER_TO_INSTANCE_FACTORY will add an entry to this map with the key that is typeID 
     * andthe value a function that will return an object (aka the newly created object).
     */
    typedef std::map<std::string, FunctionThatCreatesInstance> MapNameToFunction;



    /**
     * This class is designed to be the abstract Instance Factory. 
     * This will receive as parameters the typeID (which is a string) and 
     * a pointer to Params instance. The Params will store parameters loaded from a file.
     */
    class InstanceFactory
    {
        /** Declare it as singleton. */ 
        DeclareSingleton(InstanceFactory);

    private:

        /** This is the map with all the typeID's and the corresponding Create Function. */ 
        MapNameToFunction m_abstractUnits;

    protected:

        /** The default constructor. Protected because is singleton*/
        InstanceFactory();

        /** The default destructor. Protected because is singleton*/
        virtual ~InstanceFactory();
   
    public:

         /**
         * Used to access the map that sore typeID and corresponding CreateFuncitons
         * @returns a reference to the map.
         */
        MapNameToFunction& GetMapNameToFunction()
        {
            return m_abstractUnits;
        }

        /**
        * Function used to create an instance based on typeID and params.
        *
        * @param typeID is an std::string used to specify the id of the object (aka the name of the class).
        * @param params is an Params* pointer used to sent to the object the instance of Params with params and values.
        * @return an RTTI* pointer representing the pointer to the new created instance.
        */
        RTTI* CreateInstance(const std::string &typeID, smart_ptr<Params> params)
        {
            if(m_abstractUnits.find(typeID) != m_abstractUnits.end())
            {
                FunctionThatCreatesInstance Function = m_abstractUnits[typeID];

                return Function(params);
            }
            else
            {
                std::stringstream ss;                
                ss << "Cannot create instance for typeID=" << typeID << ". Possible causes: 1. The class " << typeID << " does not exists, 2. The class " << typeID << " exists but is not factoryable " << endl;
                LogMgr::GetInstance()->Log(LogMgr::LOG_WARNING, ss);

                return NULL;
            }
        }

        /**
        * Function used to create an instance based on typeID and params.
        *
        * @param params is an pointer used to sent to the function the params class (the parameters).
        * @return an RTTI* pointer representing the pointer to the new created instance.
        */
        RTTI* CreateInstance(smart_ptr<Params> params)
        {
            GU_ASSERT(params != NULL);

            return CreateInstance(params->GetClassTypeID(), params);
        }

    };



    /** All factoryable objects must derive this template class. */
    template<class C>
    class Factoryable: public IRTTI< Factoryable<C> >
    {
    protected:

        /** The default constructor. */
        Factoryable()
        {
        }

        /**
        * The constructor that accepts the id as parameter.
        * @param is a std::string containing the className (aka the typeID)
        */
        Factoryable(const std::string& typeID)
        {
            InstanceFactory::GetInstance()->GetMapNameToFunction()[typeID] = Factoryable<C>::CreateInstance;       
        }

        /** The default destructor. */
        virtual ~Factoryable()
        {

        }

    private:

        /**
        * Used to create an instance. This function is static , so 
        * you can call it without an instance. This is used as a callback, and will 
        * be added to the MapNameToFunction.
        *
        * @param params is a smart pointer to the instance (Params) with parameters.
        * @return a pointer to an RTTI instance.
        */
        static RTTI* CreateInstance(smart_ptr<Params> params);
    };

    
    /**
    * Used to create an instance.
    *
    * @param params is a void* pointer to the class with parameters.
    */
    template<class C>
    RTTI* Factoryable<C>::CreateInstance(smart_ptr<Params> params)
    {
        RTTI* returnVal = NEW C(params);
        return returnVal;
    }

} //namespace gu

#endif //INSTANCE_FACTORY_H
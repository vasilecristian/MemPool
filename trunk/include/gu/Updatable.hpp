/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef UPDATABLE_H
#define UPDATABLE_H

#include <fast-rtti/RTTI.h>
using namespace fastrtti;

namespace gu
{
    /** This class is used as a base class for all objects that need per frame update. */
    class Updatable: public IRTTI<Updatable>
    {
        /** used to specify if this object can be updated or not. */
        bool m_updateEnabled;

    public:

        /** the constructor */
        Updatable():m_updateEnabled(true)
        {
        }
        
        /** the destructor */
        virtual ~Updatable()
        {
        }

        /**
         * @return true if the object can be updated
         */
        inline bool IsUpdateEnabled() { return m_updateEnabled; }

        /**
        * used to enable or change the m_updateEnabled flag of this object.
        *
        * @param enableUpdate is a bool used to change the m_updateEnabled flag.
        */
        inline void EnableUpdate(bool enableUpdate) { m_updateEnabled = enableUpdate; }

        /**
        * Update is a virtual fuxntion that must be implemented by each derived class.
        *
        * @param millisFromLastCall is an int used to specify how many milliseconds passed from last call.
        */
        virtual void Update(int millisFromLastCall) = 0;
    };

}//namespace gu


#endif //UPDATABLE_H
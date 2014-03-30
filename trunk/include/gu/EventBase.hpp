/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <fast-rtti/RTTI.h>

namespace gu
{

    /** The EventBase class is the base for all events. */
    class EventBase: public fastrtti::IRTTI<EventBase>
    {
        /** m_type - represent the type of the event and must take values from enum EVENTS. */
        int m_type;

    public:
        /**
        * The EventBase constructor.
        *
        * @param type an int that specify the event type.
        */
        EventBase(int type):m_type(type)
        {
        }


        /** The EventBase destructor. */
        virtual ~EventBase()
        {
        }

        /** @return the type of this object. */
        inline int GetType()
        {
            return m_type;
        }
    };

} //namespace gu

#endif //EVENTRECEIVER_H
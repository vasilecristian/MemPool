/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <RTTI.hpp>


#include "gu/EventBase.hpp"
#include "gu/SmartPtr.hpp"

using namespace fastrtti;

namespace gu
{
    /**
     * This is the base class for any object that need to receive events.
     * If a class have EventReceiver as base class, that instance will be 
     * added to a list (a list with objects that can receive events).
     */
    class EventReceiver: public IRTTI<EventReceiver>
    {
         bool m_canProcessEvents;

    public:

        /**
         * The Constructor
         *
         * @param canProcessEvents is a bool that indicates if this receiver can or cannot process events.
         */
        EventReceiver(bool canProcessEvents):m_canProcessEvents(canProcessEvents)
        {            
        }


        /** The default Constructor */
        EventReceiver():m_canProcessEvents(true)
        {   
        }


        /** The Destructor */
        virtual ~EventReceiver()
        {
            
        }


        /** @return true if this object can process events */
        inline bool GetCanProcessEvents()
        {
            return m_canProcessEvents;
        }

        /**
         * Enable or disable the ability to process events.
         * @param a bool that specify if the object can process events or not.
         */
        inline void SetCanProcessEvents(bool canProcessEvents)
        {
            m_canProcessEvents = canProcessEvents;
        }

        /**
         * Each class derived from EventReceiver must implement the virtual function
         * OnEvent. This function will be called if there are pending events to process.
         * If this function will return true, the event will not be processed by other objects.
         *
         * @param ev is an Engine::EventBase* pointer that store information about event.
         * @return true if the event must be consumed => the event will not be sent to other receivers.
         */
        virtual bool OnEvent(gu::SmartPtr<gu::EventBase> ev) = 0;
    };

} //namespace gu


#endif //EVENTRECEIVER_H

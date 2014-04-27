/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include <vector>
#include <gu/SmartPtr.hpp>

#include "Singleton.hpp"
#include "EventReceiver.hpp"


namespace gu
{
    /**
     * This class is designed as a singleton. EventsQueue has two members which store the
     * pending events and the receivers.
     */
    class EventsQueue 
    {
        /** The Singleton declaration. */
        DeclareSingleton(EventsQueue);

        /** The vector that contains the EventReceiver objects. */
        std::vector< gu::EventReceiver* > m_receiversQ;

        /** The vector that contains the Events objects. */
        std::vector< gu::SmartPtr <gu::EventBase> > m_eventsQ;

    protected:

        /** The constructor is protected because this is a Singleton. */
        EventsQueue();
        
        /** The destructor is protected because this is a Singleton. */
        virtual ~EventsQueue();

    public:
        
        /**
         * This will add a new event to the list with pending events.
         * This event will be removed from the list after will be processed by the receivers.
         */
        void AddEvent(gu::SmartPtr<gu::EventBase> ev);

        
        /**
         * This will add a new receiver to the receivers list.
         * @param eventReceiver is a pointer to a new event receiver that need to be added to the list.
         */
        void RegisterEventReceiver(gu::EventReceiver* eventReceiver);

       
        /**
         * This will remove a receiver.
         * @param eventReceiver is a pointer to that event receiver that need to be removed from the list.
         */
        void UnRegisterEventReceiver(gu::EventReceiver* eventReceiver);

        /**
         * This will process all the pending events. Actually the events from m_eventsQ will be distributed to the 
         * events receiver from m_receiversQ.
         */
        void ProcessIncommingEvent();
       
    };


} //namespace gu

#endif //EVENTQUEUE_H
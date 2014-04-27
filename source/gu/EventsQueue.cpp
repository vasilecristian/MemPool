/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#include "gu/EventsQueue.h"

namespace gu
{

    ImplementSingleton(EventsQueue);

    EventsQueue::EventsQueue()
    {
    }

    EventsQueue::~EventsQueue()
    {
        m_eventsQ.clear();
        m_receiversQ.clear();
    }

 
    void EventsQueue::AddEvent(gu::SmartPtr <gu::EventBase> ev)
    {
        m_eventsQ.push_back(ev);
    }

    void EventsQueue::RegisterEventReceiver(gu::EventReceiver* eventReceiver)
    {
        int size = m_receiversQ.size();
        for(int i=0; i < size; i++)
        {
            if(m_receiversQ[i] == eventReceiver) 
                return;
        }

        m_receiversQ.push_back(eventReceiver);
    }

   

    void EventsQueue::UnRegisterEventReceiver(EventReceiver* eventReceiver)
    {
        int size = m_receiversQ.size();
        for(int i=0; i < size; i++)
        {
            if(m_receiversQ[i] == eventReceiver) 
            {
                m_receiversQ.erase(m_receiversQ.begin() + i);//remove object from the vector
                return;
            }
        }
    }

    void EventsQueue::ProcessIncommingEvent()
    {
    
        //sizeE - this is very important! do not optimize or delete it!
        //Because during OnEvent , the size of m_eventsQ may increase. So you need to delete only older 
        //events.
        int sizeE = m_eventsQ.size();
        int sizeR = m_receiversQ.size();


        // For each event 
        for(int j=0; j < sizeE; j++)
        {
            // For each receiver
            for(int i=0; i < sizeR; i++)
            {
                // Check if the receiver can process events
                if(m_receiversQ[i]->GetCanProcessEvents())
                {
                    ///if one of the event receivers OnEvent function returns true, the event is not 
                    ///sent to the rest of the receivers. 
                    if(m_receiversQ[i]->OnEvent(m_eventsQ[j]))
                    {
                        // So, exit the loop and go to next event.
                        break;
                    }
                }
            }
        }
    
   
        auto itr = m_eventsQ.begin();
        m_eventsQ.erase(itr, itr + sizeE);
    }
} // namespace gu
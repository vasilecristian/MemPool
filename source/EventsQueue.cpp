/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#include "EventsQueue.h"

namespace gu
{

    ImplementSingleton(EventsQueue);

    EventsQueue::EventsQueue()
    {
    }

    EventsQueue::~EventsQueue()
    {
        //remove the object and delete them (call the destructor)
        while (!m_eventsQ.empty()) 
        {
            delete (gu::EventBase*)(m_eventsQ.back());  
            m_eventsQ.pop_back();
        }

        //remove the object from the list but do not delete them
        GU_ASSERT(m_receiversQ.size() == 0);
        m_receiversQ.clear();
    }

 
    void EventsQueue::AddEvent(gu::EventBase* ev)
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

    void EventsQueue::UnRegisterEventReceiver(gu::EventReceiver* eventReceiver)
    {
        int size = m_receiversQ.size();
        for(int i=0; i < size; i++)
        {
            if(m_receiversQ[i] == eventReceiver) 
            {
                m_receiversQ.erase(m_receiversQ.begin() + i);//remove object from the vector but do not delet it
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

        bool consumeEvent = false;

        for(int i=0; i < sizeR; i++)
        {
            if(m_receiversQ[i]->GetCanProcessEvents())
            {
                for(int j=0; j < sizeE; j++)
                {
                    ///if one of the event receivers OnEvent function returns true, the event is not 
                    ///sent to the rest of the receivers.
                    consumeEvent = m_receiversQ[i]->OnEvent((EventBase*)m_eventsQ[j]);

                    if(consumeEvent) 
                        break;
                }
            }

            if(consumeEvent) 
                break;
        }
    
    
    
        for (int i = 0; i < sizeE; i++)
        {
            delete m_eventsQ[i];
        }
        auto itr = m_eventsQ.begin();
        m_eventsQ.erase(itr, itr + sizeE);
    }
} // namespace gu
/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef EVENTBASE_H
#define EVENTBASE_H

#include <fast-rtti/RTTI.h>

namespace gu
{

    /**
     * This enum represent the events used by the game.
     * You must "connect" this events with the device's events.
     */
    enum EVT
    {
        EVENT_TYPE_NONE                     = 0,

        EVENT_TYPE_MOUSE                    = (1<<0),
        EVENT_TYPE_MOUSE_MOVE               = (1<<1) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_UP                 = (1<<2) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_DOWN               = (1<<3) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_BUTTON_R           = (1<<4) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_BUTTON_M           = (1<<5) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_BUTTON_L           = (1<<6) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_DCLICK             = (1<<7) | EVENT_TYPE_MOUSE,
        EVENT_TYPE_MOUSE_WHEEL              = (1<<8) | EVENT_TYPE_MOUSE,
        

        EVENT_TYPE_TOUCH                    = (1<<9),
        EVENT_TYPE_TOUCH_BEGAN              = (1<<10) | EVENT_TYPE_TOUCH,
        EVENT_TYPE_TOUCH_END                = (1<<11) | EVENT_TYPE_TOUCH,
        EVENT_TYPE_TOUCH_MOVE               = (1<<12) | EVENT_TYPE_TOUCH,
        EVENT_TYPE_TOUCH_ZOOM               = (1<<13) | EVENT_TYPE_TOUCH,

        EVENT_TYPE_KEY                      = (1<<14),
        EVENT_TYPE_KEY_PRESSED              = (1<<15) | EVENT_TYPE_KEY,
        EVENT_TYPE_KEY_RELEASED             = (1<<16) | EVENT_TYPE_KEY,

		EVENT_TYPE_EXIT_APP	                = (1<<17),

        EVENT_TYPE_GAME_CUSTOM              = (1<<17),
    };



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
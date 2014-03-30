/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <fast-rtti/RTTI.h>
using namespace fastrtti;

namespace gu
{
    /** This class is a base class for any object that must be draw! */
    class Drawable: public IRTTI<Drawable>
    {
        
        /** depend of this , the object will be displayed or not. */
        bool m_drawEnabled;

        /**
         * If the object is be visible or not!
         * This is different from m_drawEnabled.
         * m_insideViewPort is true or false depending of the position of the object (inside or outside of viewport)
         * m_drawEnabled is controlled by the "user" - if the object can be displayed or not
         */
        bool m_insideViewPort;


    public:


        /** The Constructor (default) */
        Drawable():m_drawEnabled(true), m_insideViewPort(true){};


        /** The destructor. Note that is virtual! */    
        virtual ~Drawable(){};


        /**
        * This will tell if this object is inside viewport.
        *
        * @return a bool which is true if this object is inside viewport.
        */ 
        inline bool IsInsideViewPort() { return m_insideViewPort; }

        /**
        * this will tell if this object must be draw or not.
        *
        * @return a bool which is true if this object must be displayed.
        */ 
        inline bool IsDrawEnabled() { return m_drawEnabled; }

        /**
        * enable or disable drawing of this object
        * 
        * @param enableDraw is a bool used to enable or disable the drawing of this object.
        */         
        inline void EnableDraw(bool enableDraw) { m_drawEnabled = enableDraw; }

        /**
        * Virtual abstract function that will draw the object.
        * Must be implemented by each child(derived) class.
        *
        * @param addToDrawList is a bool used to specify if this object must added to a draw list. The default 
        *        value is false.
        * @param layerIndex is an int used to specify the index of the layer. Depending of the layer, this object 
        *        can be in front or behind other objects. The default value is -1. This can be used to FORCE a layer
        *        and ignore the layer that was set (before) for this object.
        */    
        virtual void Draw(bool addToDrawList = false, int layerIndex = -1) = 0;
    };

} //namespace gu

#endif //DRAWABLE_H
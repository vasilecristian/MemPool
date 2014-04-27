/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#ifndef _STATE_H_
#define _STATE_H_

#include <list>
#include "gu/SmartPtr.hpp"
#include "gu/EventReceiver.hpp"
#include "gu/Updatable.hpp"
#include "gu/Drawable.hpp"
#include "gu/MemPool.h"


namespace gu
{

    /**	Base class for all game states. This is used in StateStack */
    class State : public IRTTI<State>
                , public EventReceiver
                , public Updatable
                , public Drawable
                , public virtual IMemPool
    {
    public:

        /** The constructor */
		State();

        /** The destructor */
	    virtual	~State();

        /** init resources relevant to this state ..called each time you enter this state */
	    virtual int	Create();		

        /** release resources relevant to this state ...called each time you leave the state */
	    virtual void Release();					        

        /** pause this state ..from an interrupt or when coming back from a state */
        virtual void Pause();					        

        /** resumes this state ..from an interrupt or when coming back from a state */
        virtual void Resume();					        

        /** updates the game state  */
        virtual void Update(int millisFromLastCall); 

        /**  draw the game state  */
        virtual void Draw(bool addToDrawList = false, int layerIndex = -1) = 0;                         
    	
        /** this will handle the Events */
        virtual bool OnEvent(gu::SmartPtr<gu::EventBase> event);

        /** This will set the parrent for this state
         * @param parent is a pointer to a State instance that is the parent for this state.
         */
	    void SetParent(gu::SmartPtr<gu::State> parent);

        /** This function will return a pointer to the parent state. Depending of the parameter level, the pointer returned will be 
         * to the parrent of the parrent of the parrent etc...
         * If the level is 0 will be returned the parent of this state. If level is 1 , will be returned the parent of the parent.
         * @param level is an int representing what parent to be returned. 
         * @return a pointer to parent state.
         */
	    gu::SmartPtr<gu::State> GetParent( int level = 0);

        /**
         * This function is some how special. Each state must be implement this because this function will
         * be called on each frame until the Load() will return false. This is done because for states that 
         * needs to load many things, the loading must be split in loading steps to allow the progress bar to be displayed.
         * So, in Load function the loaded things must be splitted, and after the last step is finished the return false must be 
         * called, So the state machine will know not to call again the Load function.
         * @return true if loading is not finished, or false if all things was loaded.
         */
	    virtual bool Load();

        /**
         * @return true if the state is in loading. 
         */
	    bool isLoading();


        /**
         * This will set the loading flag.
         * @param loading set the loading state.
         */
	    void setLoading( bool loading );

    private:

        /** the parent */
	    gu::SmartPtr<gu::State> m_pParent;


    protected:

        /** flag that is true if loading is in progress.*/ 
	    bool m_bStateLoading;

    };

} //namespace gu


#endif	//_STATE_H_
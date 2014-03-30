/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#pragma once
#ifndef _MENUSTATES_STACK_H_
#define _MENUSTATES_STACK_H_

#include "gu/Singleton.hpp"
#include "gu/Updatable.hpp"
//#include "vsge/base/MemLeakDetector.h"
#include "gu/StateMachine/State.h"

#define STATES_STACK_SIZE	12 

namespace gu
{
    /**
     * class used to store and manipulate the states of the state machine 
     */
    class StatesStack
    {
        friend class State;

        DeclareSingleton(StatesStack);

    protected:

        /**
         * Because this class is a singleton, you cannot call the constructor. 
         * Instead you need to call GetInstance
         */
        StatesStack();
        
        /** the destructor */
        virtual ~StatesStack();
    public:

        /** Initialize the state machine */
        void		InitStateStack();

        /** Update the states machine. Change the states, Remove the states and call the Update function for current state */
	    bool        Update(int millisFromLastCall);

        /** Call the Draw function of the current state */
        void		Draw();

        /**
        * Change the state. This is used to jump from a state to another.
        * @param pState is the new state.
        * @param destroyPrevious specify if the old state must be destroyed.
        */
	    void		ChangeState(State* pState, bool destroyPrevious = true);

        /**
         * Change the state, but add it to the state stuck. Using the PopState this state will be removed from the stack.
         * @param pState is the new state.
         */
	    void		PushState(State* pState);

        /**
        * Remove the actual state from stack.
        */
	    void		PopState(bool bResume = true);

        /** Remove all states from stack */
	    void		ClearStateStack();

        /**
        * This will return the current state.
        * @return the current state.
        */
	    State*  CurrentState();

        /** 
        * This will return the previous state.
        * @return the previous state.
        */
	    State*	PreviousState();
    	
        /** @return the number ofstates from state stack */
	    inline int  GetStateNo();

        /** Does nothing right now */
	    void		ResetTouch();

        /** Does nothing right now */
	    bool		IsStateOnStack(int stateKind) const;


    private:

        /** the state that must be pushed to the stack */
        State*	m_pStatePushed;

        /** the state that must be popped from stack */
        State*	m_pStatePoped;

        /** the table with all active states. This is actually the state stack*/
	    State*	m_pStateStack[STATES_STACK_SIZE];

        /** This the previous state.*/
	    State*	m_pPreviousState;

        /** the current state index in the state stack. */
	    int			m_nStateIndex;

        /** the table with states that must be deleted */
	    State*	m_pStateStackToDelete[STATES_STACK_SIZE];

        /** the number of states that must be deleted */
	    int			m_nStateCountToDelete;

        /** This will specify that this state must be deleted. The state will be deleted the next
        * time when the Update will be called.
        * @param pState is a pointer to the state that must be deleted.
        */
	    void		MarkStateToDelete( State* pState );

        /** Delete all states that was marked to be deleted*/
	    void		DeleteStatesList();
    };

} //namespace vsge

////////////////////////////////////////////////////////////////////////////////////////////////////
#endif //_MENUSTATES_STACK_H_
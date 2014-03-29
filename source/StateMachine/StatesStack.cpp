/************************************************************************/
/*File created on 09.2012 by Cristian Vasile (vasile.cristian@gmail.com)*/
/************************************************************************/

#include <stdio.h>
#include "StateMachine/State.h"
#include "StateMachine/StatesStack.h"



namespace gu
{
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    ImplementSingleton(StatesStack);

    StatesStack::StatesStack()
    {

    }
    StatesStack::~StatesStack()
    {
        ClearStateStack();
        DeleteStatesList();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void StatesStack::InitStateStack() 
    {
        m_pStatePushed = NULL; 
        m_pStatePoped = NULL; 
        m_nStateIndex = -1; 
        m_nStateCountToDelete = 0; 
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void StatesStack::ChangeState(State* pState, bool destroyPrevious)
    {
	    // PopState	
	    GU_ASSERT(m_nStateIndex >= 0);

	    m_pPreviousState = m_pStateStack[m_nStateIndex];

	    m_pPreviousState->Release();
	    m_nStateIndex--;

	    if(destroyPrevious)
	    {
		    MarkStateToDelete(m_pPreviousState);
	    }

	    if (pState == NULL)
	    {
		    GU_ASSERT(false && "Tried to add a NULL state !!!\n");
		    ClearStateStack();
	    }

	    // PushState
	    GU_ASSERT(m_nStateIndex < STATES_STACK_SIZE);

	    m_nStateIndex++;
	    m_pStateStack[m_nStateIndex] = pState;

	    int result = pState->Create();
	    if( result < 0)
	    {
		    GU_ASSERT(false && "Failed to create the current state!!!!\n");		
		    ClearStateStack();
	    }
	    else
	    {
		    pState->Resume();
	    }

	    ResetTouch();
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    bool StatesStack::Update(int millisFromLastCall)
    {
	    if(m_pStatePoped && (m_nStateIndex >= 0))
	    {
		    State* pState = m_pStatePoped;
		    pState->Pause();
		    pState->Release();

		    m_nStateIndex--;

		    MarkStateToDelete(pState);

		    if (/*bResume*/true && m_nStateIndex >= 0)
		    {
			    m_pStateStack[m_nStateIndex]->Resume();

			    ResetTouch();

			    //if (CurrentState())
			    //{
			    //	CurrentState()->ResetControls();
			    //}
			    m_pStatePoped = NULL;
		    }
	    }

	    if(m_pStatePushed)
	    {
		    State* pState = m_pStatePushed;

		    //if (CurrentState())
		    //{
		    //	CurrentState()->ResetControls();
		    //}

		    if (pState == NULL)
		    {
			    GU_ASSERT(false && "Tried to add a NULL state !!!");
			    ClearStateStack();
		    }
		
		    GU_ASSERT(m_nStateIndex < STATES_STACK_SIZE);
		
		    State* oldState = CurrentState();
		    if (oldState)
			    oldState->Pause();

		    pState->SetParent( CurrentState() );

		    m_nStateIndex++;
		    m_pStateStack[m_nStateIndex] = pState;

		    if(pState->Create() < 0)
		    {		
			    GU_ASSERT(false && "Failed to create the current state!!!!\n");
			    ClearStateStack();
		    }
		    else
		    {
			    pState->Resume();
			    pState->setLoading(true);
		    }
		    ResetTouch();
		    m_pStatePushed = NULL;
	    }

	    DeleteStatesList();

        if(m_nStateIndex < 0) 
		    return false;

	    if (m_pStateStack[m_nStateIndex]->isLoading())
	    {
		    m_pStateStack[m_nStateIndex]->setLoading(m_pStateStack[m_nStateIndex]->Load());
	    }
	    else
	    {
		    m_pStateStack[m_nStateIndex]->Update(millisFromLastCall);
	    }

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    void StatesStack::Draw()
    {
        if(m_nStateIndex < 0) return;

        m_pStateStack[m_nStateIndex]->Draw();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void StatesStack::PushState(State* pState)
    {
	    m_pStatePushed = pState;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void StatesStack::PopState(bool bResume)
    {
	    if(m_nStateIndex < 0) return;

	    m_pStatePoped = m_pStateStack[m_nStateIndex];
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void StatesStack::ClearStateStack()
    {
	    while (m_nStateIndex >= 0)
	    {
		    State* pState = m_pStateStack[m_nStateIndex];
		
		    pState->Pause();
		    pState->Release();
		    MarkStateToDelete(pState);

		    m_nStateIndex--;
	    }

	    ResetTouch();

	    m_pStatePushed = NULL;
	    m_pStatePoped = NULL;
    }

    void StatesStack::MarkStateToDelete( State* pState )
    {
	    m_pStateStackToDelete[m_nStateCountToDelete] = pState;
	    m_nStateCountToDelete++;
    }

    void StatesStack::DeleteStatesList()
    {
	    while( m_nStateCountToDelete )
	    {
		    m_nStateCountToDelete--;
		    if( m_pStateStackToDelete[m_nStateCountToDelete] )
            {
                delete m_pStateStackToDelete[m_nStateCountToDelete];
            }
	    }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    bool StatesStack::IsStateOnStack(int stateKind) const
    {/*
	    for (int i = 0; i < _nStateIndex; i++)
	    {
		    if (_pStateStack[i]->isKindOf(stateKind))
			    return true;
	    }
    */
	    return false;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    State* StatesStack::CurrentState()
    {
	    return (m_nStateIndex >= 0) ? m_pStateStack[m_nStateIndex] : NULL;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    int  StatesStack::GetStateNo()
    { 
        return m_nStateIndex; 
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    State*	StatesStack::PreviousState() 
    { 
        return m_pPreviousState; 
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////

    void StatesStack::ResetTouch()
    {

    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

} //namespace gu
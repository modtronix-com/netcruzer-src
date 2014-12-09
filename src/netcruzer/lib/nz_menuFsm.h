/**
 * @brief           Implementation of a Finite State Machine, for a Menu
 * @file            nz_menuFsm.h
 * @author          <a href="www.modtronix.com">Modtronix Engineering</a>
 * @compiler        MPLAB XC16 compiler
 *
 * @section nz_menuFsm_desc Description
 *****************************************
 * This module implements a Finite State Machine, with special functianality for
 * implementing a menu with a Top level, and Sub Levels
 *
 * A finite state machine (FSM) or finite automation is a model of behaviour composed
 * of states, transitions and actions. A state stores information about the past, i.e.
 * it reflects the input changes from the system start to the present moment. A transition
 * indicates a state change and is described by a condition that would need to be fulfilled
 * to enable the transition. An action is a description of an activity that is to be performed
 * at a given moment. There are several action types:
 * <b> Entry action </b>
 *   - execute the action when entering the state
 * <b> Exit action </b>
 *   - execute the action when exiting the state
 * <b> Input action </b>
 *   - execute the action dependent on present state and input conditions
 * <b> Transition action </b>
 *   - execute the action when performing a certain transition
 *
 * For details see:<br>
 * http://en.wikipedia.org/wiki/Finite_state_machine
 *
 *
 * @subsection nz_menuFsm_conf Configuration
 *****************************************
 * The following defines are used to configure this module, and should be placed in projdefs.h. Note
 * that all items marked [-DEFAULT-] are defaults, and do not have to be placed in projdefs.h if they
 * contain desired configuration! For details, see @ref info_conf_proj "Project Configuration".
 @code
 // *********************************************************************
 // -------------- FSM State Machine Configuration --------------------
 // *********************************************************************

 @endcode
 *********************************************************************/

 /*********************************************************************
 * @section nz_menuFsm_lic Software License Agreement
 *
 * The software supplied herewith is owned by Modtronix Engineering, and is
 * protected under applicable copyright laws. The software supplied herewith is
 * intended and supplied to you, the Company customer, for use solely and
 * exclusively on products manufactured by Modtronix Engineering. The code may
 * be modified and can be used free of charge for commercial and non commercial
 * applications. All rights are reserved. Any use in violation of the foregoing
 * restrictions may subject the user to criminal sanctions under applicable laws,
 * as well as to civil liability for the breach of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED IN AN 'AS IS' CONDITION. NO WARRANTIES, WHETHER EXPRESS,
 * IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE
 * COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 **********************************************************************
 * File History
 *
 * 2005-10-06, David Hosken (DH):
 *    - Initial version
 *********************************************************************/

#ifndef _fsmMenu_H_
#define _fsmMenu_H_

typedef struct _FSM {
    uint8_t stateTop;   //Current state
    uint8_t stateSub1;  //State of sub1 Sub Menu, 0 means it is disabled
    uint8_t action;     //Current action -  is a SMT_ACTION_XXX define
} FSM;

#define FSM_ACTION_ENTRY   0    //Action to take when entering the state
#define FSM_ACTION_EXIT    1    //Action to take when existing the state
#define FSM_ACTION_INPUT   2    //Execute the action dependent on present state and input conditions. Will normally spend most time in this state!
#define FSM_ACTION_TRANS   3    //Execute the action when performing a certain transition

/**
 * Change action of current state to FSM_ACTION_ENTRY. The state is not changed.
 */
#define fsmSetAction2Entry(sm) {sm.action = FSM_ACTION_ENTRY;}

/**
 * Change action of current state to FSM_ACTION_EXIT. The state is not changed.
 */
#define fsmSetAction2Exit(sm) {sm.action = FSM_ACTION_EXIT;}

/**
 * Change action of current state to FSM_ACTION_INPUT. The state is not changed.
 */
#define fsmSetAction2Input(sm) {sm.action = FSM_ACTION_INPUT;}

/**
 * Change action of current state to FSM_ACTION_TRANS. The state is not changed.
 */
#define fsmSetAction2Transition(sm) {sm.action = FSM_ACTION_TRANS;}

/**
 * Set the Top State to the given state. Disable Sub1 state (set to 0). Sets the action to ENTRY.
 */
#define fsmSetState(sm, newState) {sm.stateTop=newState; sm.stateSub1=0; sm.action=FSM_ACTION_ENTRY;}

/**
 * Increment Top State. Disable Sub1 state (set to 0). Sets the action to ENTRY.
 */
#define fsmIncState(sm, lastState) { (sm.stateTop==lastState) ? sm.stateTop=0 : sm.stateTop++; sm.stateSub1=0; sm.action=FSM_ACTION_ENTRY;}

/**
 * Decrement Top State. Disable Sub1 state (set to 0). Sets the action to ENTRY.
 */
#define fsmDecState(sm, lastState) { (sm.stateTop==0) ? sm.stateTop=lastState : sm.stateTop--; sm.stateSub1=0; sm.action=FSM_ACTION_ENTRY;}

/**
 * Set the Sub1 State to the given state. Sets the action to ENTRY.
 */
#define fsmSetStateSub1(sm, newState) {sm.stateSub1=newState; sm.action=FSM_ACTION_ENTRY;}

/**
 * Increment Sub1 State. Sets the action to ENTRY.
 */
#define fsmIncStateSub1(sm, lastState) { (sm.stateSub1==lastState) ? sm.stateSub1=1 : sm.stateSub1++; sm.action=FSM_ACTION_ENTRY;}

/**
 * Decrement Sub1 State. Sets the action to ENTRY.
 */
#define fsmDecStateSub1(sm, lastState) { (sm.stateSub1==1) ? sm.stateSub1=lastState : sm.stateSub1--; sm.action=FSM_ACTION_ENTRY;}
#endif


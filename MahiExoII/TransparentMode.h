#pragma once
#include "StateMachine.h"
#include "Clock.h"
#include "Daq.h"
#include "MahiExoII.h"

using namespace mel;

class TransparentModeData : public EventData {

public:

};

class TransparentMode : public StateMachine {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    TransparentMode(Clock& clock, Daq* daq, MahiExoII& meii);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TRANSPARENT,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const NoEventData*);
    StateAction<TransparentMode, NoEventData, &TransparentMode::sf_init> sa_init;

    void sf_transparent(const NoEventData*);
    StateAction<TransparentMode, NoEventData, &TransparentMode::sf_transparent> sa_transparent;

    void sf_stop(const NoEventData*);
    StateAction<TransparentMode, NoEventData, &TransparentMode::sf_stop> sa_stop;

    // STATE MAP
    virtual const StateMapRow* get_state_map() {
        static const StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_transparent,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // USER INPUT CONTROL
    void wait_for_input();
    bool check_stop();
    bool stop_ = false;

    //-------------------------------------------------------------------------
    // TRANSPARENT MODE COMPONENTS
    //-------------------------------------------------------------------------

    // HARDWARE CLOCK
    Clock clock_;

    // HARDWARE
    Daq* daq_;
    MahiExoII meii_;

    // GENERIC STATE FUNCTIONS
    void sf_transparent_start();
    void sf_transparent_step();
    void sf_transparent_stop();

    //-------------------------------------------------------------------------
    // TRANSPARENT MODE
    //-------------------------------------------------------------------------

    // STATE TRANSITION EVENTS

    // USEFUL STATE VARIABLES
    double st_enter_time_;


};
#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoII.h"

class EmgTrainingData : public mel::EventData {

public:

    

};

class EmgTraining : public mel::StateMachine {

public:

    EmgTraining(mel::Clock& clock, mel::Daq* daq, MahiExoII* exo);


private:

    mel::Daq* daq_;
    MahiExoII* exo_;

    enum States {
        ST_INIT,
        ST_NEUTRAL,
        ST_STOP,
        ST_NUM_STATES
    };

    // Define the state machine state functions with event data type
    void ST_Init(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Init> Init;

    void ST_Neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Neutral> Neutral;

    void ST_Stop(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Stop> Stop;

    // State map to define state object order. Each state map entry defines a
    // state object.
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &Init,
            &Neutral,
            &Stop,
        };
        return &STATE_MAP[0];
    }

};
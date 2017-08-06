#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoII.h"

class EmgTrainingData : public mel::EventData {

public:

};

class EmgTraining : public mel::StateMachine {

public:

    EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoII* exo);


private:

    mel::Daq* q8_emg_;
    mel::Daq* q8_ati_;
    MahiExoII* exo_;
    Eigen::VectorXd raw_force_;
    Eigen::MatrixXd calib_mat_;
    Eigen::VectorXd calib_force_;

    enum States {
        ST_INIT,
        ST_TO_NEUTRAL,
        ST_HOLD_NEUTRAL,
        ST_STOP,
        ST_NUM_STATES
    };

    // Define the state machine state functions with event data type
    void ST_Init(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Init> Init;

    void ST_To_Neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_To_Neutral> To_Neutral;

    void ST_Hold_Neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Hold_Neutral> Hold_Neutral;

    void ST_Stop(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::ST_Stop> Stop;

    // State map to define state object order. Each state map entry defines a
    // state object.
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &Init,
            &To_Neutral,
            &Hold_Neutral,
            &Stop,
        };
        return &STATE_MAP[0];
    }

    void ctrl_c_task() override;

};
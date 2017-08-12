#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoIIFrc.h"
#include "util.h"
#include "mahiexoii_util.h"
#include "MelShare.h"
#include "Filter.h"

class EmgTrainingData : public mel::EventData {

public:

};

class EmgTraining : public mel::StateMachine {

public:

    EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIFrc* meii);

private:
    
    enum States {
        ST_INIT,
        ST_TO_NEUTRAL,
        ST_HOLD_NEUTRAL,
        ST_STOP,
        ST_NUM_STATES
    };

    // Define the state machine state functions with event data type
    void sf_init(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_init> sa_init;

    void sf_to_neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_to_neutral> sa_to_neutral;

    void sf_hold_neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_hold_neutral> sa_hold_neutral;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_stop> sa_stop;

    // State map to define state object order. Each state map entry defines a
    // state object.
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_to_neutral,
            &sa_hold_neutral,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // HARDWARE
    mel::Daq* q8_emg_;
    mel::Daq* q8_ati_;
    MahiExoIIFrc* meii_;

    // HARDWARE CLOCK
    mel::Clock clock_;


    Eigen::VectorXd raw_force_;
    //Eigen::MatrixXd calib_mat_;
    Eigen::VectorXd calib_force_;


    mel::double_vec set_points_;
    mel::double_vec kp_;
    mel::double_vec kd_;
    mel::double_vec init_pos_;
    mel::double_vec goal_pos_;
    double init_time_;
    mel::double_vec speed_;
    mel::double_vec x_ref_;
    mel::double_vec new_torques_;
    mel::int8_vec backdrive_;

    // FILTERING
    mel::Filter lpf_;
    mel::Filter multi_lpf_;

    // MELShare(s)
    mel::share::MelShare pos_data_ = mel::share::MelShare("MEII_pos");
    mel::double_vec data_p_;
    mel::share::MelShare ati_data_ = mel::share::MelShare("MEII_ati");
    mel::double_vec force_share_;
    mel::double_vec force_filt_;

};
#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoIIEmg.h"
#include "MelShare.h"
#include "Filter.h"
#include "Clock.h"
#include "util.h"
#include "mahiexoii_util.h"

class EmgTrainingData : public mel::EventData {

public:

};

class EmgTraining : public mel::StateMachine {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIEmg* meii);

private:
    
    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TO_NEUTRAL,
        ST_HOLD_NEUTRAL,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_init> sa_init;

    void sf_to_neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_to_neutral> sa_to_neutral;

    void sf_hold_neutral(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_hold_neutral> sa_hold_neutral;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_to_neutral,
            &sa_hold_neutral,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // USER INPUT CONTROL
    bool check_stop();
    bool stop_ = false;

    //-------------------------------------------------------------------------
    // EXPERIMENT SETUP
    //-------------------------------------------------------------------------


    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------

    

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* q8_emg_;
    mel::Daq* q8_ati_;
    MahiExoIIEmg* meii_;

    // MEII PARAMETERS
    mel::int8_vec backdrive_ = mel::int8_vec(5, 0);

    // MEII POSITION CONTROL
    mel::double_vec kp_ = { 35, 7, 25, 30, 3500 };
    mel::double_vec kd_ = { 0.25, 0.06, 0.05, 0.08, 5 };
    mel::double_vec init_pos_ = mel::double_vec(5, 0);
    mel::double_vec goal_pos_ = mel::double_vec(5, 0);
    double init_time_ = 0.0;
    mel::double_vec speed_ = { 0.25, 0.25, 0.125, 0.125, 0.0125 };
    mel::double_vec x_ref_ = mel::double_vec(5, 0);
    mel::double_vec set_points_ = mel::double_vec(5, 0);
    mel::double_vec new_torques_ = mel::double_vec(5, 0);
    mel::share::MelShare pos_data_ = mel::share::MelShare("MEII_pos");
    mel::double_vec data_p_ = mel::double_vec(5, 0);

    // FORCE SENSING
    mel::double_vec force_share_ = mel::double_vec(6, 0);
    mel::double_vec force_filt_ = mel::double_vec(6, 0);
    mel::Filter multi_lpf_ = mel::Filter(6, 4, { 0.009735570656078, -0.032135367809242, 0.045449986329302, -0.032135367809242, 0.009735570656078 }, { 1.000000000000000, -3.572942808701423, 4.807914652718555, -2.886325158284144, 0.652003706289986 });
    mel::share::MelShare ati_data_ = mel::share::MelShare("MEII_ati");


    // EMG SENSING
    mel::double_mat emg_data_window_;
    
    


    
    
    

};
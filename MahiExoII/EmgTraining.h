#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoIIEmgFrc.h"
#include "MelShare.h"
#include "Filter.h"
#include "Clock.h"
#include "util.h"
#include "mahiexoii_util.h"
#include "GuiFlag.h"

class EmgTrainingData : public mel::EventData {

public:

};

class EmgTraining : public mel::StateMachine {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    EmgTraining(mel::Clock& clock, mel::Daq* q8_emg, mel::Daq* q8_ati, MahiExoIIEmgFrc& meii, GuiFlag& gui_flag, int input_mode);

private:
    
    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TO_CENTER,
        ST_HOLD_CENTER,
        ST_PRESENT_TARGET,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_init> sa_init;

    void sf_to_center(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_to_center> sa_to_center;

    void sf_hold_center(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_hold_center> sa_hold_center;

    void sf_present_target(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_present_target> sa_present_target;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<EmgTraining, mel::NoEventData, &EmgTraining::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_to_center,
            &sa_hold_center,
            &sa_present_target,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // USER INPUT CONTROL
    int INPUT_MODE_;
    void wait_for_input();
    bool check_stop();
    bool stop_ = false;

    //-------------------------------------------------------------------------
    // EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    mel::double_vec target_tol_ = { 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 0.01 };

    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------

    // GUI FLAGS
    GuiFlag& gui_flag_;

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* q8_emg_;
    mel::Daq* q8_ati_;
    MahiExoIIEmgFrc meii_;

    // MEII PARAMETERS
    mel::int8_vec backdrive_ = { 0,1,1,1,1 };

    // MEII POSITION CONTROL
    mel::double_vec kp_ = { 35, 7, 25, 30, 0 };
    mel::double_vec kd_ = { 0.25, 0.06, 0.05, 0.08, 0 };
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
    mel::array_2D<double,8,200> emg_data_window_;
    
    // EXPERIMENT CONTROL VARIABLES
    mel::char_vec target_check_joint_ = {1,1,1,1,1};
    bool target_reached_ = false;
    bool check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec check_joint);

    // UNITY INPUT/OUTPUT
    mel::share::MelShare scene_num_ = mel::share::MelShare("scene_num");
    mel::share::MelShare target_ = mel::share::MelShare("target");
    mel::share::MelShare force_mag_ = mel::share::MelShare("force_mag");
  
};
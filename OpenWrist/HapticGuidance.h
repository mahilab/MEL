#pragma once
#include "StateMachine.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include "Pendulum.h"
#include "GuiFlag.h"

class HapticGuidance : public mel::StateMachine {

public:

    HapticGuidance(mel::Clock& clock, OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_START,
        ST_STEP,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE MACHINE FUNCTIONS AND STATE ACTIONS
    void ST_Start(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::ST_Start> Start;

    void ST_Step(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::ST_Step> Step;

    void ST_Stop(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::ST_Stop> Stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &Start,
            &Step,
            &Stop,
        };
        return &STATE_MAP[0];
    }

    void ctrl_c_task() override;

    //-------------------------------------------------------------------------
    // HAPTIC GUIDANCE EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    // FLAGS
    GuiFlag& gui_flag_;

    // HARDWARE
    mel::Daq* daq_;
    OpenWrist* ow_;
    Cuff cuff_;

    // CUFF PARAMETERS
    short int offset[2];
    short int scaling_factor[2];

    // PENDULUM 
    Pendulum pendulum;

    // TRAJECTORY
    mel::share::MelShare trajectory = mel::share::MelShare("trajectory");
    mel::share::MelShare exp_pos = mel::share::MelShare("exp_pos");
    std::array<double, 20> trajectory_data = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    std::array<int, 2> exp_pos_data = { 0, 0 };
    void dumb_solution(std::array<int, 2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix);

};
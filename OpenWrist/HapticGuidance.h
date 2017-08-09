#pragma once
#include "StateMachine.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include "Pendulum.h"
#include "GuiFlag.h"

//----------------------------------------------------------------------------
// Hatpic guidance experiements with OpenWrist, CUFF, and MahiExo-II 
//----------------------------------------------------------------------------
// Evan Pezent, Simone Fanni, Josh Bradely (August 2017 - October 2017)
//----------------------------------------------------------------------------
// Condition 0: OpenWrist P/S w/ Perlin Noise Forces                  x10 subj
// Condition 1: OpenWrist P/S + CUFF w/ Perlin Noise Forces           x10 subj
// Condition 2: OpenWrist P/S + CUFF w/ Haptic Guidance Forces        x10 subj
// Condition 3: OpenWrist P/S + MahiExo-II w/ Haptic Guidance Forces  x10 subj
//----------------------------------------------------------------------------
// State Machine Flow (per subject+condition)
//----------------------------------------------------------------------------
// EXPERIMENT BREAKDOWN              | BLOCK | TRIAL #s |
//----------------------------------------------------------------------------
// Questionaire / Initialize Devices |       |          |
// Familiarization Trial (x1 - 1min) | F     | 1        |
// Evaluation Trials (x3 - 20s ea)   | E1    | 2 - 4    |
// Training Trials (x12 - 20s ea)    | T1    | 5 - 16   |
// Evaluation Trials (x3 - 20s ea)   | E2    | 17 - 19  |
// Training Trials (x12 - 20s ea)    | T2    | 20	31  |
// Evaluation Trials (x3 - 20s ea)   | E3    | 32 - 34  |
// Training Trials (x12 - 20s ea)    | T3    | 35 - 46  |
// 5 Minute Break                    | B     |          |
// Evaluation Trials (x3 - 20s ea)   | E4    | 47 - 49  |
// Training Trials (x12 - 20s ea)    | T4    | 50 - 61  |
// Evaluation Trials (x3 - 20s ea)   | E5    | 62 - 64  |
// Training Trials (x12 - 20s ea)    | T5    | 65 - 76  | 
// Evaluation Trials (x3 - 20s ea)   | E6    | 77 - 79  |
// Training Trials (x12 - 20s ea)    | T6    | 80 - 91  |
// Evaluation Trials (x3 - 20s ea)   | E7    | 92 - 94  |
// Generalization (x12 - 20s ea)     | G     | 95 - 106 |
//----------------------------------------------------------------------------

class HapticGuidance : public mel::StateMachine {

public:

    HapticGuidance(mel::Clock& clock, OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag, int subject_number, int condition, int input_mode);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_FAMILIARIZATION,
        ST_EVALUATION,
        ST_TRAINING,
        ST_BREAK,
        ST_GENERALIZATION,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS AND STATE ACTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_init> sa_init;

    void sf_familiarization(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_familiarization> sa_familiarization;

    void sf_evaluation(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_evaluation> sa_evaluation;

    void sf_training(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_training> sa_training;

    void sf_break(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_break> sa_break;

    void sf_generalization(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_generalization> sa_generlization;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_familiarization,
            &sa_evaluation,
            &sa_training,
            &sa_break,
            &sa_generlization,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    void ctrl_c_task() override;

    // INPUT CONTROL
    int INPUT_MODE_;
    void wait_for_continue_input();
    void allow_continue_input();

    //-------------------------------------------------------------------------
    // HAPTIC GUIDANCE EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    int SUBJECT_NUMBER_;
    int CONDITION_;
    int NUM_FAMILIARIZATION_TRIALS_ = 1;
    int NUM_EVALUATION_TRIALS_ = 3;
    int NUM_TRAINING_TRIALS_ = 12;
    int NUM_GENERALIZATION_TRIALS = 12;

    double FAMILIARIZATION_LENGTH = 5.0;
    double EVALUATION_LENGTH = 5.0;
    double TRAINING_LENGTH = 20.0;
    double BREAK_LENGTH = 300.0;
    double GENERALIZATION_LENGTH = 20.0;

    std::string DIRECTORY_;

    // FLAGS
    GuiFlag& gui_flag_;

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* daq_;
    OpenWrist* ow_;
    Cuff cuff_;

    // CUFF PARAMETERS
    short int offset[2];
    short int scaling_factor[2];

    // PENDULUM 
    Pendulum pendulum;

    // TRAJECTORY VARIABLES
    mel::share::MelShare trajectory_x = mel::share::MelShare("trajectory_x", 54*4); 
    mel::share::MelShare trajectory_y = mel::share::MelShare("trajectory_y", 54*4);
    mel::share::MelShare exp_pos = mel::share::MelShare("exp_pos");
    std::array<int, 54> trajectory_x_data;
    std::array<int, 54> trajectory_y_data;
    std::array<int, 2> exp_pos_data = { 0, 0 };
    void estimate_expert_position(std::array<int, 2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix);
    double find_error_angle(double actual_angle, std::array<int, 2> intersection_pix, double length_m);

};
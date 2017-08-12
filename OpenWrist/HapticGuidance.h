#pragma once
#include "StateMachine.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include "Pendulum.h"
#include "GuiFlag.h"
#include <noise/noise.h>

//----------------------------------------------------------------------------
// Hatpic guidance experiements with OpenWrist, CUFF, and MahiExo-II 
//----------------------------------------------------------------------------
// Evan Pezent, Simone Fanni, Josh Bradely (August 2017 - October 2017)
//----------------------------------------------------------------------------
// Condition 1: OpenWrist P/S w/ Perlin Noise Forces                  x10 subj
// Condition 2: OpenWrist P/S + CUFF w/ Perlin Noise Forces           x10 subj
// Condition 3: OpenWrist P/S + CUFF w/ Haptic Guidance Forces        x10 subj
// Condition 4: OpenWrist P/S + MahiExo-II w/ Haptic Guidance Forces  x10 subj
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

    HapticGuidance(mel::Clock& clock, mel::Daq* daq, OpenWrist& open_wrist, Cuff& cuff, GuiFlag& gui_flag, int input_mode,
        int subject_number, int condition, std::string start_trial = "F1-1");

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_START,
        ST_FAMILIARIZATION,
        ST_EVALUATION,
        ST_TRAINING,
        ST_BREAK,
        ST_GENERALIZATION,
        ST_TRANSITION,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_start(const mel::NoEventData*);
    void sf_familiarization(const mel::NoEventData*);
    void sf_evaluation(const mel::NoEventData*);
    void sf_training(const mel::NoEventData*);
    void sf_break(const mel::NoEventData*);
    void sf_generalization(const mel::NoEventData*);
    void sf_transition(const mel::NoEventData*);
    void sf_stop(const mel::NoEventData*);

    // STATE ACTIONS
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_start> sa_start;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_familiarization> sa_familiarization;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_evaluation> sa_evaluation;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_training> sa_training;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_break> sa_break;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_generalization> sa_generlization;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_transition> sa_transition;
    mel::StateAction<HapticGuidance, mel::NoEventData, &HapticGuidance::sf_stop> sa_stop;
    
    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_start,
            &sa_familiarization,
            &sa_evaluation,
            &sa_training,
            &sa_break,
            &sa_generlization,
            &sa_transition,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // INPUT CONTROL
    int INPUT_MODE_;
    void wait_for_continue_input();
    void allow_continue_input();

    //-------------------------------------------------------------------------
    // EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    // SUBJECT/CONDITION
    int SUBJECT_NUMBER_;
    int CONDITION_;

    // BLOCK TYPES
    enum BlockType {
        FAMILIARIZATION = 0,
        EVALUATION = 1,
        TRAINING = 2,
        BREAK = 3,
        GENERALIZATION = 4
    };

    // EXPERIMENT BLOCK ORDER (SET MANUALLY)
    std::vector<BlockType> BLOCK_ORDER_ = {
        FAMILIARIZATION,
        EVALUATION, TRAINING, 
        EVALUATION, TRAINING, 
        EVALUATION, TRAINING,
        BREAK,
        EVALUATION, TRAINING, 
        EVALUATION, TRAINING, 
        EVALUATION, TRAINING,
        EVALUATION, GENERALIZATION 
    };

    std::array<std::string, 5> BLOCK_TAGS_ = { "F", "E", "T", "B", "G" };

    // NUMBER OF BLOCKS PER BLOCK TYPE (SET DURING CONSTRUCTION)
    // [ FAMILIARIZATION, EVALUATION, TRAINING, BREAK, GENERALIZATION ]
    std::array<int, 5> NUM_BLOCKS_ = { 0,0,0,0,0 };

    // NUMBER OF TRIALS PER BLOCK TYPE PER BLOCK NUMBER (SET MANUALLY)
    // [ FAMILIARIZATION, EVALUATION, TRAINING, BREAK, GENERALIZATION ]
    std::array<int, 5> NUM_TRIALS_ = { 1, 3, 12, 1, 12 };

    // LENGTH IN SECONDS OF EACH BLOCK TYPE TRIAL (SET MANUALLY)
    // [ FAMILIARIZATION, EVALUATION, TRAINING, BREAK, GENERALIZATION ]
    std::array<double, 5> LENGTH_TRIALS_ = { 10, 10, 10, 10, 10 };

    // EXPERIMENT TRIAL ORDERING
    void build_experiment_();
    int current_trial_index_ = 0;
    std::vector<BlockType> TRIALS_ORDER_;
    std::vector<std::string> TRIALS_NAMES_;
    BlockType get_next_trial();

    // SUBJECT DIRECTORY
    std::string DIRECTORY_;

    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------

    // GUI FLAGS
    GuiFlag& gui_flag_;

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* daq_;
    OpenWrist& open_wrist_;
    Cuff& cuff_;

    // CUFF PARAMETERS
    short int CUFF_NORMAL_FORCE_ = 5;
    short int CUFF_NOISE_GAIN_ = 8400;
    short int CUFF_GUIDANCE_GAIN_ = 12000;
    short int offset[2];
    short int scaling_factor[2];

    // PENDULUM 
    Pendulum pendulum_;

    // TRAJECTORY VARIABLES
    double sin_freq_ = 0.1;
    double cos_freq_ = 0.2;
    mel::share::MelShare trajectory_x_ = mel::share::MelShare("trajectory_x", 54*4); 
    mel::share::MelShare trajectory_y_ = mel::share::MelShare("trajectory_y", 54*4);
    mel::share::MelShare exp_pos = mel::share::MelShare("exp_pos");
    std::array<int, 54> trajectory_x_data;
    std::array<int, 54> trajectory_y_data;
    std::array<int, 2> exp_pos_data = { 0, 0 };
    void estimate_expert_position(std::array<int, 2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix);
    double find_error_angle(double actual_angle, std::array<int, 2> intersection_pix, double length_m);

    void update_trajectory_and_expert();

    // UNITY GAMEMANAGER
    mel::share::MelShare unity_ = mel::share::MelShare("unity");
    // [ background, pendulum on/off , trajectory region on/off , trajectory center on/off, expert on/off, radius on/off , stars on/off ]
    std::array<int, 7> unity_data_ = { 1,1,1,1,1,1,1 };

    // PERLIN NOISE MODULES
    noise::module::Perlin perlin_module_;

};
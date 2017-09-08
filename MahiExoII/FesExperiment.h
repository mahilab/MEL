#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoII.h"
#include "Filter.h"
#include "Clock.h"
#include "util.h"
#include "MelShare.h"
#include "DataLog.h"

// Simple udp client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <stdio.h>
#include <winsock2.h>
#include <iostream>
#include <SDKDDKVer.h>
#pragma comment(lib,"ws2_32.lib") //Winsock Library
#include <stdio.h>
#include <tchar.h>

#define SERVER "169.254.10.128"  //ip address of udp server
#define BUFLEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data

class FesExperimentData : public mel::EventData {

public:

};

class FesExperiment : public mel::StateMachine {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    FesExperiment(mel::Clock& clock, mel::Daq* q8_emg, mel::MahiExoII& meii, int subject_number, int trial);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TRANSPARENT,
        ST_TO_EXTENDED,
        ST_HOLD_EXTENDED,
        ST_FLEXION_TRAJECTORY,
        ST_HOLD_FLEXED,
        ST_EXTENSION_TRAJECTORY,
        ST_FINISH,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_init> sa_init;

    void sf_transparent(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_transparent> sa_transparent;

    void sf_to_extended(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_to_extended> sa_to_extended;

    void sf_hold_extended(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_hold_extended> sa_hold_extended;

    void sf_flexion_trajectory(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_flexion_trajectory> sa_flexion_trajectory;

    void sf_hold_flexed(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_hold_flexed> sa_hold_flexed;

    void sf_extension_trajectory(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_extension_trajectory> sa_extension_trajectory;

    void sf_finish(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_finish> sa_finish;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_transparent,
            &sa_to_extended,
            &sa_hold_extended,
            &sa_flexion_trajectory,
            &sa_hold_flexed,
            &sa_extension_trajectory,
            &sa_finish,
            &sa_stop,
        };
        return &STATE_MAP[0];
    }

    // USER INPUT CONTROL
    void wait_for_input();
    bool check_stop();
    bool stop_ = false;

    //-------------------------------------------------------------------------
    // EXPERIMENT SETUP
    //-------------------------------------------------------------------------

    int current_target_ = 0;
    double init_transparent_time_ = 1.0; // [s]
    mel::char_vec target_check_joint_ = { 1, 1, 1, 1, 1 };
    mel::double_vec target_tol_ = { 2.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 0.01 };
    double hold_extended_time_ = 1.0; // time to hold at start of trajectory [s]
    double hold_flexed_time_ = 1.0; // time to hold at end of trajectory [s]
    mel::double_vec extended_pos_ = { -60 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD,  0.09 };
    mel::double_vec flexed_pos_ = { -10 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD,  0.09 };
    double elbow_extended_pos_ = extended_pos_[0];
    double elbow_flexed_pos_ = flexed_pos_[0];
    double flexion_trajectory_time_ = 5.0; // time of trajectory from start to finish [s] 
    double extension_trajectory_time_ = 5.0; // time of trajectory from start to finish [s]
    

    // SUBJECT DIRECTORY
    std::string DIRECTORY_;

    // RANDOMIZED CONDITION ORDER
    std::vector<std::vector<double>> cond_mat_ = { { 3, 0, 1, 0, 2, 2, 1, 2, 1, 0, 0, 1, 0, 2, 2, 1, 0, 2, 1, 1, 2, 0, 1, 0, 1, 0, 2, 2 },
                                                   { 3, 1, 1, 0, 2, 1, 0, 0, 2, 2, 1, 2, 0, 0, 1, 2, 1, 0, 2, 2, 1, 1, 0, 0, 2, 0, 1, 2 },
                                                   { 3, 0, 2, 1, 0, 2, 2, 1, 0, 1, 1, 0, 0, 1, 2, 0, 2, 1, 2, 1, 2, 2, 0, 1, 1, 0, 2, 0 },
                                                   { 3, 2, 1, 2, 1, 2, 0, 1, 0, 0, 2, 0, 1, 2, 0, 2, 1, 0, 1, 2, 0, 1, 1, 2, 1, 0, 2, 0 },
                                                   { 3, 2, 1, 0, 1, 0, 2, 1, 0, 2, 0, 0, 2, 1, 2, 1, 0, 1, 2, 1, 2, 0, 1, 0, 0, 2, 2, 1 },
                                                   { 3, 2, 2, 1, 1, 1, 0, 2, 0, 0, 0, 1, 1, 0, 0, 2, 2, 1, 2, 2, 2, 0, 0, 1, 0, 1, 2, 1 },
                                                   { 3, 1, 2, 2, 0, 1, 2, 0, 0, 1, 0, 2, 2, 1, 1, 0, 0, 1, 2, 0, 1, 1, 2, 0, 0, 2, 1, 2 },
                                                   { 3, 2, 2, 2, 1, 0, 0, 0, 1, 1, 0, 2, 2, 2, 1, 1, 0, 1, 0, 2, 2, 1, 0, 0, 2, 1, 0, 1 },
                                                   { 3, 0, 1, 0, 1, 2, 1, 0, 2, 2, 0, 1, 1, 1, 2, 0, 0, 2, 2, 1, 2, 0, 1, 0, 0, 2, 2, 1 },
                                                   { 3, 1, 2, 2, 2, 1, 0, 0, 0, 1, 1, 2, 1, 2, 0, 1, 0, 2, 0, 2, 2, 1, 0, 0, 1, 0, 1, 2 } };


    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------

    // SUBJECT/CONDITION
    int SUBJECT_NUMBER_;
    int TRIAL_;
    int CONDITION_;

    // DATA LOG
    mel::DataLog log_ = mel::DataLog("fes_exp_log",false);
    std::vector<double> log_data_;
    void log_row();

    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* q8_emg_;
    mel::MahiExoII meii_;

    // MEII PARAMETERS
    mel::int8_vec backdrive_ = { 0,1,1,1,1 }; // anatomical joints; 1 = backdrivable, 0 = active

    // UDP
    double elbow_pos_deg_ = 0.0; 
    double elbow_ref_pos_deg_ = 0.0;
    double current_time_ = 0.0;
    struct sockaddr_in si_other_;
    int s_ = sizeof(si_other_);
    int slen_ = sizeof(si_other_);  
    double UDP_data_[5];
    void FesExperiment::send_udp_packet(double elbow_pos_deg);

    // MEII POSITION CONTROL
    mel::double_vec kp_ = { 50, 7, 25, 30, 0 };
    mel::double_vec kd_ = { 0.25, 0.06, 0.05, 0.08, 0 };
    mel::double_vec init_pos_ = mel::double_vec(5, 0);
    mel::double_vec goal_pos_ = mel::double_vec(5, 0);
    double init_time_ = 0.0;
    mel::double_vec speed_ = { 0.25, 0.25, 0.125, 0.125, 0.0125 };
    mel::double_vec x_ref_ = mel::double_vec(5, 0);
    mel::double_vec set_points_ = mel::double_vec(5, 0);
    mel::double_vec new_torques_ = mel::double_vec(5, 0);

    // FORCE SENSING
    mel::double_vec commanded_torques_ = mel::double_vec(5, 0);

    // STATE TRANSITION EVENTS
    bool first_udp_packet_sent = false;
    bool init_transparent_time_reached_ = false;
    bool target_reached_ = false;
    bool initial_position_reached_ = false;
    bool hold_extended_time_reached_ = false;
    bool flexion_trajectory_finished_ = false;
    bool hold_flexed_time_reached_ = false;
    bool extension_trajectory_finished_ = false;
    
    // USEFUL STATE VARIABLES
    double st_enter_time_;

    // UTILITY FUNCTIONS
    bool check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec check_joint, bool print_output = false);
    bool check_wait_time_reached(double wait_time, double init_time, double current_time);
    double compute_elbow_anatomical_position(double robot_elbow_position);
    double compute_elbow_flexion_trajectory(double init_time, double current_time);
    double compute_elbow_extension_trajectory(double init_time, double current_time);
    
    // MELSCOPE VARIABLES
    mel::share::MelShare pos_share_ = mel::share::MelShare("pos_share");
    mel::share::MelShare vel_share_ = mel::share::MelShare("vel_share");
    mel::share::MelShare torque_share_ = mel::share::MelShare("torque_share");
    mel::share::MelShare ref_share_ = mel::share::MelShare("ref_share");
};
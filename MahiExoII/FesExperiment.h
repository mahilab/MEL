#pragma once
#include "StateMachine.h"
#include "Q8Usb.h"
#include "MahiExoII.h"
#include "Filter.h"
#include "Clock.h"
#include "util.h"
#include "mahiexoii_util.h"

// Simple udp client
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include<stdio.h>
#include<winsock2.h>
# include <iostream>
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

    FesExperiment(mel::Clock& clock, mel::Daq* q8_emg, MahiExoII& meii);

private:

    //-------------------------------------------------------------------------
    // STATE MACHINE SETUP
    //-------------------------------------------------------------------------

    // STATES
    enum States {
        ST_INIT,
        ST_TRANSPARENT,
        ST_TO_CENTER,
        ST_HOLD_CENTER,
        ST_FINISH,
        ST_STOP,
        ST_NUM_STATES
    };

    // STATE FUNCTIONS
    void sf_init(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_init> sa_init;

    void sf_transparent(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_transparent> sa_transparent;

    void sf_to_center(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_to_center> sa_to_center;

    void sf_hold_center(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_hold_center> sa_hold_center;

    void sf_finish(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_finish> sa_finish;

    void sf_stop(const mel::NoEventData*);
    mel::StateAction<FesExperiment, mel::NoEventData, &FesExperiment::sf_stop> sa_stop;

    // STATE MAP
    virtual const mel::StateMapRow* get_state_map() {
        static const mel::StateMapRow STATE_MAP[] = {
            &sa_init,
            &sa_transparent,
            &sa_to_center,
            &sa_hold_center,
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
    double init_transparent_time_ = 5.0; // [s]
    std::vector<int> target_sequence_ = { 1, 2, 1, 2 };
    mel::char_vec target_check_joint_ = { 1,1,1,1,1 };
    mel::double_vec target_tol_ = { 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 1.0 * mel::DEG2RAD, 0.01 };
    double hold_center_time_ = 1.0; // time to hold at center target [s]
    double force_mag_goal_ = 1000.0; // [N^2]
    double force_mag_tol_ = 100.0; // [N]
    double force_mag_dwell_time_ = 1.0; // [s]
    double force_mag_maintained_ = 0.0; // [s]
    double force_mag_time_now_ = 0.0;
    double force_mag_time_last_ = 0.0;

    mel::double_vec center_pos_ = { -35 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD, 0 * mel::DEG2RAD,  0.09 };

    //-------------------------------------------------------------------------
    // EXPERIMENT COMPONENTS
    //-------------------------------------------------------------------------


    // HARDWARE CLOCK
    mel::Clock clock_;

    // HARDWARE
    mel::Daq* q8_emg_;
    MahiExoII meii_;

    // MEII PARAMETERS
    mel::int8_vec backdrive_ = { 0,1,1,1,1 }; // anatomical joints; 1 = backdrivable, 0 = active

    // UDP
    double elbow_pos_deg_ = 0; 
    double current_time_ = 0;
    struct sockaddr_in si_other_;
    int s_ = sizeof(si_other_);
    int slen_ = sizeof(si_other_);  
    double UDP_data_[2];
    

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
    bool init_transparent_time_reached_ = false;
    bool target_reached_ = false;
    bool hold_center_time_reached_ = false;
    bool force_mag_reached_ = false;
    bool emg_data_processed_ = false;
    bool end_of_target_sequence_ = false;
    bool check_target_reached(mel::double_vec goal_pos, mel::double_vec current_pos, mel::char_vec check_joint, bool print_output = false);
    bool check_wait_time_reached(double wait_time, double init_time, double current_time);
    bool check_force_mag_reached(double force_mag_goal, double force_mag);

    // USEFUL STATE VARIABLES
    double st_enter_time_;


};
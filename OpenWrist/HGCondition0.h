#pragma once
#include "Task.h"
#include "MelShare.h"
#include "Integrator.h"
#include <array>
#include "util.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include "Pendulum.h"
#include "GuiFlag.h"

class HGCondition0 : public mel::Task {

 public:

     HGCondition0(OpenWrist* open_wrist, mel::Daq* daq, GuiFlag& gui_flag) : Task("hg_condition_0"), ow_(open_wrist), daq_(daq), gui_flag_(gui_flag) {}
     
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

     void start() override;
     void step() override;
     void stop() override;

     void bisection_solution(std::array<int, 2>& coordinates_pix, double time, double amplitude_sc_m, double freq_sine, double freq_cosine, double length_m, double joint_pos_y_pix);
     double find_error_angle(double actual_angle, int* intersection_pix, double length_m);

};
#pragma once
#include <vector>
#include "util.h"
#include "Joint.h"

// FORWARD DECLARATION(S)
class Daq;

class Robot {

public:

    // constructor
    Robot(uint num_joints);

    // parameters
    const uint num_joints_;
    double_vec joint_positions_;

    std::vector<Joint> joints_;
    

    virtual double_vec get_joint_positions();
    void set_joint_torques(double_vec new_joint_torques);

};
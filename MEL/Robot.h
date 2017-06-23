#pragma once
#include <vector>
#include "Joint.h"
#include "RevoluteJoint.h"
#include "Actuator.h"
#include "Encoder.h"
#include "Daq.h"

class Robot {

public:

    // parameters
    const uint num_joints_;

    // constructor
    Robot(uint num_joints)
        : num_joints_(num_joints) {}

    std::vector<Joint> joints_;
    
    /*
    std::vector<Joint*> joints_ptrs_;
    std::vector<Actuator> actuators_;
    std::vector<Encoder> encoders_;
    std::vector<Daq> daqs_;
    */
    
    void get_joint_positions(double_vec joint_positions);
    void set_joint_torques(double_vec joint_torques);
    
};
#pragma once
#include <vector>
#include "Joint.h"
#include "RevoluteJoint.h"
#include "Actuator.h"
#include "Encoder.h"
#include "Daq.h"
#include "util.h"

class Robot {

public:

    // parameters
    const uint num_joints_;
    double_vec joint_positions_;

    // constructor
    Robot(Daq* daq, uint num_joints);

    std::vector<Joint> joints_;

    double_vec get_joint_positions();
    void set_joint_torques(double_vec new_joint_torques);

};
#pragma once
#include "Daq.h"
#include "Joint.h"
#include "RevoluteJoint.h"
#include "PrismaticJoint.h"
#include "Actuator.h"
#include "Encoder.h"
#include "util.h"
#include <vector>

namespace mel {

    class Robot {

    public:

        // constructor
        Robot() {};
        Robot(std::vector<Daq*> daqs, uint_vec ai_channels, uint_vec ao_channels, uint_vec di_channels, uint_vec do_channels, uint_vec enc_channels);

        // components
        std::vector<Daq*> daqs_;
        std::vector<Joint*> joints_;
        std::vector<Encoder*> encoders_;
        std::vector<Actuator*> actuators_;

        // getters and setters of state variables
        double_vec get_joint_positions();
        double_vec get_joint_velocities();
        double_vec set_joint_torques(double_vec new_torques);


    private:

        // state variables
        double_vec joint_positions_;
        double_vec joint_velocities_;
        double_vec joint_torques_;

    };
}
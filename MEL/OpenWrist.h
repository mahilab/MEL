#pragma once
#include "Robot.h"

namespace mel {

    class OpenWrist : public Robot {

    public:

        OpenWrist();
        OpenWrist(Daq::FullChannelSet joint_0, Daq::FullChannelSet joint_1, Daq::FullChannelSet joint_2);

        Daq* daq_;

        std::vector<Encoder> encoders_;
        std::vector<Actuator> actuators_;
        std::vector<RevoluteJoint> joints_;


    };
}
#pragma once
#include "Robot.h"

namespace mel {

    class OpenWrist : public Robot {

    public:

        OpenWrist();

        Daq* daq_;

        std::vector<Encoder> encoders_;
        std::vector<Actuator> actuators_;
        std::vector<RevoluteJoint> joints_;


    };
}
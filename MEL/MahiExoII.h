#pragma once
#include "Robot.h"

namespace mel {

    class MahiExoII : public Robot {

    public:

        // constructor
        MahiExoII(Daq* e0_daq, uint e0_chan, Daq* e1_daq, uint e1_chan,
            Daq* ao0_daq, uint ao0_chan, Daq* ao1_daq, uint ao1_chan,
            Daq* do0_daq, uint do0_chan, Daq* do1_daq, uint do1_chan);

        // parameters
        const uint e0_chan_;
        const uint e1_chan_;
        const uint ao0_chan_;
        const uint ao1_chan_;
        const uint do0_chan_;
        const uint do1_chan_;

        // references
        Daq* e0_daq_;
        Daq* e1_daq_;
        Daq* ao0_daq_;
        Daq* ao1_daq_;
        Daq* do0_daq_;
        Daq* do1_daq_;
        RevoluteJoint joint0_;
        RevoluteJoint joint1_;
        //std::vector<Joint> joints_;
        Encoder encoder0_;
        Encoder encoder1_;
        Actuator actuator0_;
        Actuator actuator1_;



        //double_vec get_joint_positions();

    };
}
#pragma once
#include "Robot.h"

namespace mel {

    class MahiExoII : public Robot {

    public:

        // constructor
        MahiExoII(Daq* daq, uint_vec ai_channels, uint_vec ao_channels, uint_vec di_channels, uint_vec do_channels, uint_vec enc_channels);

        // destructor
        ~MahiExoII();

        // parameters
    

        // references
        std::vector<Daq*> daqs_;
        std::vector<Joint*> joints_;
        std::vector<Encoder*> encoders_;
        std::vector<Actuator*> actuators_;


        //double_vec get_joint_positions();

    };
}
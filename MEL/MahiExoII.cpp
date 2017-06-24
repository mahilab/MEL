#include "MahiExoII.h"

namespace mel {

    MahiExoII::MahiExoII(Daq* daq, uint_vec ai_channels, uint_vec ao_channels, uint_vec di_channels, uint_vec do_channels, uint_vec enc_channels) :
        Robot(5),
        daq_(daq)
    {

        encoders_.push_back(new Encoder(INCH2METER*0.42, 2048, 4, daq, enc_channels[0]));
        encoders_.push_back(new Encoder(INCH2METER*0.17, 2048, 4, daq, enc_channels[1]));
        encoders_.push_back(new Encoder(INCH2METER*0.23, 2048, 4, daq, enc_channels[2]));
        encoders_.push_back(new Encoder(INCH2METER*0.23, 2048, 4, daq, enc_channels[3]));
        encoders_.push_back(new Encoder(INCH2METER*0.23, 2048, 4, daq, enc_channels[4]));

        actuators_.push_back(new Actuator(INCH2METER*0.42, 0.127, 6.0, 1.8, daq, ao_channels[0], do_channels[0]));
        actuators_.push_back(new Actuator(INCH2METER*0.17, 0.0603, 3.17, 1.8, daq, ao_channels[1], do_channels[1]));
        actuators_.push_back(new Actuator(INCH2METER*0.23, 0.175, 0.626, 0.184, daq, ao_channels[2], do_channels[2]));
        actuators_.push_back(new Actuator(INCH2METER*0.23, 0.175, 0.626, 0.184, daq, ao_channels[3], do_channels[3]));
        actuators_.push_back(new Actuator(INCH2METER*0.23, 0.175, 0.626, 0.184, daq, ao_channels[4], do_channels[4]));

        joints_.push_back(new RevoluteJoint(INCH2METER*4.5, encoders_[0], actuators_[0]));
        joints_.push_back(new RevoluteJoint(INCH2METER*2.5, encoders_[1], actuators_[1]));
        joints_.push_back(new PrismaticJoint(encoders_[2], actuators_[2]));
        joints_.push_back(new PrismaticJoint(encoders_[3], actuators_[3]));
        joints_.push_back(new PrismaticJoint(encoders_[4], actuators_[4]));

    }

    MahiExoII::~MahiExoII() {

        for (Encoder* encoder : encoders_) {
            delete encoder;
        }

        for (Actuator* actuator : actuators_) {
            delete actuator;
        }

        for (Joint* joint : joints_) {
            delete joint;
        }

    }

    /*
    double_vec MahiExoII::get_joint_positions() {

    }
    */

}
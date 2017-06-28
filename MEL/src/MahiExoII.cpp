#include "MahiExoII.h"

namespace mel {

    MahiExoII::MahiExoII(Daq* daq, uint_vec ai_channels, uint_vec ao_channels, uint_vec di_channels, uint_vec do_channels, uint_vec enc_channels) :
        Robot(5),
        daq_(daq),
        qp_({0.25*PI, 0.25*PI, 0.25*PI, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0, 0, 0.1305}),
        qs_({0.1305, 0.1305, 0.1305}),
        psi_({ 0.25*PI, 0.25*PI, 0.25*PI, 0.1305, 0.1305, 0.1305, 0, 0, 0, 0, 0, 0.1305 })
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
        joints_.push_back(new RevoluteJoint());
        joints_.push_back(new RevoluteJoint());
        joints_.push_back(new RevoluteJoint());
        joints_.push_back(new PrismaticJoint());
        joints_.push_back(new PrismaticJoint());
        joints_.push_back(new PrismaticJoint());
        joints_.push_back(new RevoluteJoint());
        joints_.push_back(new RevoluteJoint());
        joints_.push_back(new RevoluteJoint());

        joint_positions_ = double_vec(joints_.size(), 0.0);
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

    
    double_vec MahiExoII::get_joint_positions() {
        for (auto it = joints_.begin(); it != joints_.end(); ++it)
            if ( (*it)->encoder_ != nullptr ) {
                (*it)->update_position();
            }
            
        forward_kinematics(qs_);

        for (auto it = joints_.begin(); it != joints_.end(); ++it)
            joint_positions_[it - joints_.begin()] = joints_[it - joints_.begin()]->position_;

        return joint_positions_;
    }

    void MahiExoII::forward_kinematics(double_vec qs) {
        
        psi_ = psi_func(qp_, qs_, R_, r_, a56_, alpha5_, alpha13_);
        psi_d_qp_ = psi_d_qp_func(qp_, r_, alpha5_, alpha13_);

    }

    double_vec MahiExoII::psi_func(double_vec qp, double_vec qs, double R, double r, double a56, double alpha5, double alpha13) {
        return { qp[3] * sin(qp[0]) - qp[9] - r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
                 R*cos(alpha5) - qp[10] - a56*sin(alpha5) - qp[3] * cos(alpha5)*cos(qp[0]) - r*cos(alpha13)*cos(qp[7])*cos(qp[8]) + r*cos(qp[7])*sin(alpha13)*sin(qp[8]),
                 a56*cos(alpha5) - qp[11] + R*sin(alpha5) - qp[3] * sin(alpha5)*cos(qp[0]) - r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
                 qp[4] * sin(qp[1]) - qp[9] - r*cos(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
                 R*cos(alpha5 - (2 * PI) / 3) - qp[10] - a56*sin(alpha5 - (2 * PI) / 3) - qp[4] * cos(alpha5 - (2 * PI) / 3)*cos(qp[1]) - r*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3) + r*cos(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3),
                 a56*cos(alpha5 - (2 * PI) / 3) - qp[11] + R*sin(alpha5 - (2 * PI) / 3) - qp[4] * cos(qp[1])*sin(alpha5 - (2 * PI) / 3) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
                 qp[5] * sin(qp[2]) - qp[9] - r*cos((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])),
                 R*cos((2 * PI) / 3 + alpha5) - qp[10] - a56*sin((2 * PI) / 3 + alpha5) - qp[5] * cos((2 * PI) / 3 + alpha5)*cos(qp[2]) - r*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13) + r*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13),
                 a56*cos((2 * PI) / 3 + alpha5) - qp[11] + R*sin((2 * PI) / 3 + alpha5) - qp[5] * cos(qp[2])*sin((2 * PI) / 3 + alpha5) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])),
                 qp[3]-qs[0],
                 qp[4]-qs[1],
                 qp[5]-qs[2] };
    }
    
    double_vec MahiExoII::psi_d_qp_func(double_vec qp, double r, double alpha5, double alpha13) {
        return { qp[3] * cos(qp[0]), 0, 0, sin(qp[0]), 0, 0, -r*cos(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(alpha13)*cos(qp[7])*cos(qp[8]) - r*cos(qp[6])*cos(qp[7])*sin(alpha13)*sin(qp[8]), r*sin(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                 qp[3] * cos(alpha5)*sin(qp[0]), 0, 0, -cos(alpha5)*cos(qp[0]), 0, 0, 0, r*cos(alpha13)*cos(qp[8])*sin(qp[7]) - r*sin(alpha13)*sin(qp[7])*sin(qp[8]), r*cos(alpha13)*cos(qp[7])*sin(qp[8]) + r*cos(qp[7])*cos(qp[8])*sin(alpha13), 0, -1, 0,
                 qp[3] * sin(alpha5)*sin(qp[0]), 0, 0, -sin(alpha5)*cos(qp[0]), 0, 0, r*cos(alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin(alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(alpha13)*sin(qp[8]) - r*cos(alpha13)*cos(qp[7])*cos(qp[8])*sin(qp[6]), r*sin(alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos(alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                 0, qp[4] * cos(qp[1]), 0, 0, sin(qp[1]), 0, -r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3) - r*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3), r*sin(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                 0, qp[4] * cos(alpha5 - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(alpha5 - (2 * PI) / 3)*cos(qp[1]), 0, 0, r*cos(qp[8])*cos(alpha13 - (2 * PI) / 3)*sin(qp[7]) - r*sin(qp[7])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3), r*cos(qp[7])*cos(qp[8])*sin(alpha13 - (2 * PI) / 3) + r*cos(qp[7])*cos(alpha13 - (2 * PI) / 3)*sin(qp[8]), 0, -1, 0,
                 0, qp[4] * sin(alpha5 - (2 * PI) / 3)*sin(qp[1]), 0, 0, -cos(qp[1])*sin(alpha5 - (2 * PI) / 3), 0, r*cos(alpha13 - (2 * PI) / 3)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin(alpha13 - (2 * PI) / 3) - r*cos(qp[7])*cos(qp[8])*cos(alpha13 - (2 * PI) / 3)*sin(qp[6]), r*sin(alpha13 - (2 * PI) / 3)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos(alpha13 - (2 * PI) / 3)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                 0, 0, qp[5] * cos(qp[2]), 0, 0, sin(qp[2]), -r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[6])*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13) - r*cos(qp[6])*cos(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13), r*sin((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), -1, 0, 0,
                 0, 0, qp[5] * cos((2 * PI) / 3 + alpha5)*sin(qp[2]), 0, 0, -cos((2 * PI) / 3 + alpha5)*cos(qp[2]), 0, r*cos(qp[8])*cos((2 * PI) / 3 + alpha13)*sin(qp[7]) - r*sin(qp[7])*sin(qp[8])*sin((2 * PI) / 3 + alpha13), r*cos(qp[7])*cos(qp[8])*sin((2 * PI) / 3 + alpha13) + r*cos(qp[7])*cos((2 * PI) / 3 + alpha13)*sin(qp[8]), 0, -1, 0,
                 0, 0, qp[5] * sin((2 * PI) / 3 + alpha5)*sin(qp[2]), 0, 0, -cos(qp[2])*sin((2 * PI) / 3 + alpha5), r*cos((2 * PI) / 3 + alpha13)*(sin(qp[6])*sin(qp[8]) - cos(qp[6])*cos(qp[8])*sin(qp[7])) + r*sin((2 * PI) / 3 + alpha13)*(cos(qp[8])*sin(qp[6]) + cos(qp[6])*sin(qp[7])*sin(qp[8])), r*cos(qp[7])*sin(qp[6])*sin(qp[8])*sin((2 * PI) / 3 + alpha13) - r*cos(qp[7])*cos(qp[8])*cos((2 * PI) / 3 + alpha13)*sin(qp[6]), r*sin((2 * PI) / 3 + alpha13)*(cos(qp[6])*sin(qp[8]) + cos(qp[8])*sin(qp[6])*sin(qp[7])) - r*cos((2 * PI) / 3 + alpha13)*(cos(qp[6])*cos(qp[8]) - sin(qp[6])*sin(qp[7])*sin(qp[8])), 0, 0, -1,
                 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 };
    }

    /*
    void MahiExoII::set_joint_torques(double joint_torques) {
        joints_[joint]->torque_ = joint_torque;
    }
    */
}
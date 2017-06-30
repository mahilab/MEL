#include "OpenWrist.h"

namespace mel {

    OpenWrist::OpenWrist() :
        Robot(3)
    {
        encoders_.push_back(Encoder(0.234, 500, 4, daq_, 0));
        encoders_.push_back(Encoder(0.234, 500, 4, daq_, 1));
        encoders_.push_back(Encoder(0.117, 500, 4, daq_, 2));

        actuators_.push_back(Actuator(0.234, 0.0603, 1, 3.17, daq_, 0, 0));
        actuators_.push_back(Actuator(0.234, 0.0603, 1, 3.17, daq_, 0, 0));
        actuators_.push_back(Actuator(0.117, 0.0538, 1, 1.74, daq_, 0, 0));

        joints_.push_back(RevoluteJoint(4.375, &encoders_[0], &actuators_[0]));
        joints_.push_back(RevoluteJoint(4.500, &encoders_[1], &actuators_[1]));
        joints_.push_back(RevoluteJoint(3.000, &encoders_[2], &actuators_[2]));    
    }   
}
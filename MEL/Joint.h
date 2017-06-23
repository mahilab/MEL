#pragma once
#include <string>
#include "Encoder.h"
#include "Actuator.h"

class Joint {

public:    
  
    // references
    Encoder* encoder_;
    Actuator* actuator_;

    // constructor
    Joint(Encoder* encoder, Actuator* actuator);
    
    // state variables
    double position_;
    double velocity_;
    double torque_;
    
    // getters of state variables
    double get_position();
    double get_velocity();

    // setters of other objects' state variables
    void set_actuator_torque(double joint_torque);

    // pure virtual functions 
    virtual double encoder_to_joint_space(double counts) = 0;
    virtual double joint_torque_to_actuator_torque(double joint_torque) = 0;
    
    
};
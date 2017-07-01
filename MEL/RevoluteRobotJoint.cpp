#include "RevoluteRobotJoint.h"

namespace mel {

    RevoluteRobotJoint::RevoluteRobotJoint(PositionSensor* position_sensor, double position_sensor_transmission, Actuator* actuator, double actuator_transmission) :
        RobotJoint(position_sensor, position_sensor_transmission, actuator, actuator_transmission)
    {
    }

    /*
    double RevoluteJoint::encoder_to_joint_space(double counts) {
        return 2.0 * PI * encoder_->radius_ / (radius_ * encoder_->counts_per_revolution_ * encoder_->quadrature_factor_) * counts;
    }

    double RevoluteJoint::joint_torque_to_actuator_torque(double joint_torque) {
        return (actuator_->radius_ / radius_) * joint_torque;
    }
    */
}
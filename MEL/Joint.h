#pragma once
#include "Device.h"
#include "PositionSensor.h"
#include "Actuator.h"

namespace mel {

    class Joint : public Device {

    public:

        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        /// Default constructor
        Joint();
        /// Constructor for joint without position, velocity, or torque limits
        Joint(std::string name, PositionSensor* position_sensor, double position_sensor_transmission, 
            Actuator* actuator, double actuator_transmission);
        /// Constructor for joint with position, velocity, and torque limits
        Joint(std::string name, PositionSensor* position_sensor, double position_sensor_transmission, 
            Actuator* actuator, double actuator_transmission,
            std::array<double,2> position_limits, double velocity_limit, double torque_limit, bool saturate = true);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        virtual void enable() override;
        virtual void disable() override;

        /// Converts PositionSensor position to RobotJoint position
        double get_position(); 
        /// Converts PositionSensor velocity to RobotJoint velocity
        double get_velocity(); 
        /// Returns last set joint torque
        double get_torque();   
        /// Sets the joint torque
        void set_torque(double new_torque);

        /// Gets current position, checks it against limits, and returns true if min or max exceeded, false otherwise
        bool check_position_limits();
        /// Gets current velocity, checks it against limit, and returns true if exceeded, false otherwise
        bool check_velocity_limit();
        ///  Gets last commanded torque, checks it against torque limit, and returns true if exceeded, false otherise
        bool check_torque_limit();
        /// Gets current position, velocity, and torque, checks them against limits, and returns true if either exceeded, false otherwise
        bool check_all_limits();

        //---------------------------------------------------------------------
        // PUBLIC VARIABLES
        //---------------------------------------------------------------------

        PositionSensor* position_sensor_;
        Actuator* actuator_;
        
        const double position_sensor_transmission_;
        const double actuator_transmission_;

        std::array<double, 2> position_limits_; ///< the [min, max] position limits of the Joint
        double velocity_limit_; ///< the absolute limit on the Joint's velocity
        bool has_torque_limit_; ///< whether or not the Joint should enforce torque limits

        bool saturate_; ///< command torques will be saturated at the torque limit if this is true

    protected:

        //---------------------------------------------------------------------
        // PROTECTED VARIABLES
        //---------------------------------------------------------------------

        double position_; ///< the stored position of the Joint since the last call to get_position()
        bool has_position_limits_; ///< whether or not the Joint should check position limits

        double velocity_; ///< the stored velocity of the Joint since the last call to get_velocity()
        bool has_velocity_limit_; ///< whether or not the Joint should check velocity limits

        double torque_; ///< the stored torque of the Joint since the last call to set_torque()
        double torque_limit_; ///< the absolute limit on torque that should be allowed to the Joint

    };
}
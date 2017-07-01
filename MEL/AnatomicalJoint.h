#pragma once

namespace mel {

    class AnatomicalJoint {

    public:

        // constructor
        AnatomicalJoint();

        // getters of state variables
        double get_position();
        double get_velocity();
        void set_torque(double joint_torque);

    private:

        double position_;
        double velocity_;
        double torque_;

    };
}
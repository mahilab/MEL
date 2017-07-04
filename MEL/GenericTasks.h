#pragma once
#include "Robot.h"
#include "Task.h"
#include <functional>

namespace mel {

    class ReadController : public Task {

    };

    class PdJointController : public Task {

    public:

        PdJointController(RobotJoint* joint, double kp, double kd, std::function<double(double)> x_ref, std::function<double(double)> xd_ref) :
            joint_(joint), kp_(kp), kd_(kd), x_ref_(x_ref), xd_ref_(xd_ref) {}

        RobotJoint* joint_;
        double kp_;
        double kd_;
        std::function<double(double)> x_ref_;
        std::function<double(double)> xd_ref_;

        void start() override { }
        void step() override {
            double torque = kp_ * (x_ref_(time()) - joint_->get_position()) + kd_ * (xd_ref_(time()) - joint_->get_velocity());
            joint_->set_torque(torque);
        }
        void stop() { }

    };

}
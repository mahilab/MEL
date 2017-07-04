#pragma once
#include "Robot.h"
#include "Task.h"
#include <functional>

namespace mel {

    class ReaderTask : public Task {
    public:
        ReaderTask(Daq* daq) : Task("daq_reader_" + daq->name_), daq_(daq) {}
        Daq* daq_;
        void step() override { daq_->read_all(); }
    };

    class WriterTask : public Task {
    public:
        WriterTask(Daq* daq) : Task("daq_writer_" + daq->name_), daq_(daq) {}
        Daq* daq_;
        void step() override { daq_->write_all(); }
    };

    class PdJointController : public Task {
    public:

        PdJointController(RobotJoint* joint, double kp, double kd, std::function<double(double)> x_ref, std::function<double(double)> xd_ref) :
            Task("pd_controller_" + joint->name_), joint_(joint), kp_(kp), kd_(kd), x_ref_(x_ref), xd_ref_(xd_ref) {}

        RobotJoint* joint_;
        double kp_;
        double kd_;
        std::function<double(double)> x_ref_;
        std::function<double(double)> xd_ref_;

        void step() override {
            double torque = kp_ * (x_ref_(time()) - joint_->get_position()) + kd_ * (xd_ref_(time()) - joint_->get_velocity());
            joint_->set_torque(torque);
        }

    };

    class ClockTester : public Task {
    public:
        ClockTester() : Task("clock_tester") {}
        void start() override { std::cout << "Starting ClockTester" << std::endl; }
        void step() override {   }
        void stop() override { std::cout << "Stopping ClockTester" << std::endl; }
    };

}
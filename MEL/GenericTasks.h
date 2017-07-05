#pragma once
#include "Robot.h"
#include "Exo.h"
#include "Task.h"
#include <functional>

namespace mel {

    class DaqReaderTask : public Task {
    public:
        DaqReaderTask(Daq* daq) : Task("daq_reader_" + daq->name_), daq_(daq) {}
        Daq* daq_;
        void step() override { daq_->read_all(); }
    };

    class DaqWriterTask : public Task {
    public:
        DaqWriterTask(Daq* daq) : Task("daq_writer_" + daq->name_), daq_(daq) {}
        Daq* daq_;
        void step() override { daq_->write_all(); }
    };

    class DaqReloaderTask : public Task {
    public:
        DaqReloaderTask(Daq* daq) : Task("daq_reloader_" + daq->name_), daq_(daq) {}
        Daq* daq_;
        void step() override { daq_->reload_watchdog(); }
    };

    class StartExo : public Task {
    public:
        StartExo(Exo* exo, Daq* daq) : Task("exo_start"), exo_(exo), daq_(daq) {}
        Exo* exo_;
        Daq* daq_;
        void start() override {
            std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
            getchar();
            daq_->activate();
            daq_->zero_encoders();
            std::cout << "Press ENTER to enable Exo <" << exo_->name_ << ">" << std::endl;
            getchar();
            exo_->enable();
            std::cout << "Press Enter to start the controller" << std::endl;
            daq_->start_watchdog(0.1);
        }
    };

    class StopExo : public Task {
    public:
        StopExo(Exo* exo, Daq* daq) : Task("exo_stop"), exo_(exo), daq_(daq) {}
        Exo* exo_;
        Daq* daq_;
        void stop() override {
            exo_->disable();
            daq_->deactivate();
        }
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
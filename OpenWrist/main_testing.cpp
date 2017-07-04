#include <iostream>
#include "GenericTasks.h"
#include "Task.h"
#include "ControlLoop.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include <functional>
#include <Windows.h>

// Controller implementation minimum working example
class MyController : public mel::Task {

public:

    MyController(OpenWrist& open_wrist, mel::Daq* daq) : open_wrist_(open_wrist), daq_(daq) {}
    OpenWrist open_wrist_;
    mel::Daq* daq_;

    void start() override {
        std::cout << "Starting MyController" << std::endl;     
        std::cout << "Press ENTER to activate " << daq_->name_ << std::endl;
        getchar();
        daq_->activate();
        daq_->zero_encoders();
        std::cout << "Press ENTER to enable the OpenWrist." << std::endl;
        getchar();
        open_wrist_.enable();
        std::cout << "Press ENTER to start the controller." << std::endl;
        daq_->start_watchdog(0.1);
    }
    void step() override {
        daq_->read_all();
        daq_->reload_watchdog();
        open_wrist_.robot_joints_[2]->set_torque(0.345);
        open_wrist_.robot_joints_[1]->set_torque(0.345);
        daq_->write_all();
        daq_->log_data(time());
    }
    void stop() override {
        std::cout << "Stopping MyController" << std::endl;
        daq_->deactivate();
    }
    void pause() override {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() override {
        std::cout << "Resuming MyController" << std::endl;
    }
};



double sin_trajectory(double amplitude, double frequency, double time) {
    return amplitude * sin(2 * mel::PI * frequency * time);
}


class ClockTester : public mel::Task {
    void start() override { std::cout << "Starting ClockTest" << std::endl; }
    void step() override {   }
    void stop() override { std::cout << "Stopping ClockTester" << std::endl; }
};


int main(int argc, char * argv[]) {  

    HANDLE hThread = GetCurrentThread();
    SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);

    //  create a Q8Usb object
    mel::uint32 id = 0;

    mel::channel_vec  ai_channels = { 0, 1, 2 };
    mel::channel_vec  ao_channels = { 0, 1, 2 };
    mel::channel_vec  di_channels = { 0, 1, 2 };
    mel::channel_vec  do_channels = { 0, 1, 2 };
    mel::channel_vec enc_channels = { 0, 1, 2 };

    mel::Q8Usb::Options options;
    options.update_rate_ = mel::Q8Usb::Options::UpdateRate::Fast_8kHz;
    options.decimation_ = 1;
    options.ao_modes_[0] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options.ao_modes_[1] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options.ao_modes_[2] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0,  1.912, 18.43, 0, -1, 0, 1000);
    mel::Daq* q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

    // create and configure an OpenWrist object
    OpenWrist::Config config;
    for (int i = 0; i < 3; i++) {
        config.enable_[i] = q8->do_(i);
        config.command_[i] = q8->ao_(i);
        config.encoder_[i] = q8->encoder_(i);
        config.encrate_[i] = q8->encrate_(i);
        config.amp_gains_[i] = 1;
    }    

    OpenWrist open_wrist(config);

    // create a controller, clock, and loop
    mel::Task* my_controller = new MyController(open_wrist, q8);
    mel::Task* clock_tester = new ClockTester();
    auto traj0 = std::bind(sin_trajectory, 80 * mel::DEG2RAD, 0.25, std::placeholders::_1);
    auto traj1 = std::bind(sin_trajectory, 60 * mel::DEG2RAD, 0.25, std::placeholders::_1);
    auto traj2 = std::bind(sin_trajectory, 30 * mel::DEG2RAD, 0.25, std::placeholders::_1);
    mel::Task* pd_controller0 = new mel::PdJointController(open_wrist.robot_joints_[0], 20, 1, traj0, 0);
    mel::Task* pd_controller1 = new mel::PdJointController(open_wrist.robot_joints_[1], 20, 1, traj1, 0);
    mel::Task* pd_controller2 = new mel::PdJointController(open_wrist.robot_joints_[2], 20, 1, traj2, 0);
    mel::Clock my_clock(1000, true);
    mel::Controller my_loop(my_clock);

    // queue controllers
    my_loop.queue_task(pd_controller0);
    my_loop.queue_task(pd_controller1);
    my_loop.queue_task(pd_controller2);

    // execute the controller
    my_loop.execute(); 

    int i = 0;

    // delete controller
    delete my_controller;
    delete clock_tester;

    return 0;
}



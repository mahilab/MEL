#include <iostream>
#include "GenericTasks.h"
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include <functional>
#include <Windows.h>

// Controller implementation minimum working example
class OpenWristController : public mel::Task {

public:

    OpenWristController(OpenWrist& open_wrist, mel::Daq* daq) : Task("open_wrist_controller"), open_wrist_(open_wrist), daq_(daq) {}
    OpenWrist open_wrist_;
    mel::Daq* daq_;

    void start() override {
        std::cout << "Starting OpenWrist Controller" << std::endl;     
        std::cout << "Press ENTER to activate DAQ" << daq_->name_ << std::endl;
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
        daq_->reload_watchdog();
        daq_->log_data(time());
    }
    void stop() override {
        daq_->deactivate();
    }
    
};

double sin_trajectory(double amplitude, double frequency, double time) {
    return amplitude * sin(2 * mel::PI * frequency * time);
}

int main(int argc, char * argv[]) {  

    // set Windows thread priority
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms685100(v=vs.85).aspx
    HANDLE hThread = GetCurrentThread();
    SetPriorityClass(hThread, HIGH_PRIORITY_CLASS); // use REALTIME_PRIORITY_CLASS with extreme care!
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

    // create some trajectoeries for OpenWrist to follow
    auto traj0 = std::bind(sin_trajectory, 80 * mel::DEG2RAD, 0.25, std::placeholders::_1); // 80*sin(2*pi*0.25*t)
    auto traj1 = std::bind(sin_trajectory, 60 * mel::DEG2RAD, 0.25, std::placeholders::_1);
    auto traj2 = std::bind(sin_trajectory, 30 * mel::DEG2RAD, 0.25, std::placeholders::_1);

    // make a new Clock and Controller
    mel::Clock clock(1000, true); // 1000 Hz, clock logging enabled
    mel::Controller controller(clock);

    // create tasks for our Controller
    mel::Task* ow_ctrl = new OpenWristController(open_wrist, q8);
    mel::Task* reader = new mel::ReaderTask(q8);
    mel::Task* pd_controller0 = new mel::PdJointController(open_wrist.robot_joints_[0], 20, 1, traj0, 0);
    mel::Task* pd_controller1 = new mel::PdJointController(open_wrist.robot_joints_[1], 20, 1, traj1, 0);
    mel::Task* pd_controller2 = new mel::PdJointController(open_wrist.robot_joints_[2], 20, 1, traj2, 0);
    mel::Task* writer = new mel::WriterTask(q8);

    // queue Tasks for the Controller to execute
    controller.queue_task(ow_ctrl);
    controller.queue_task(reader);
    controller.queue_task(pd_controller0);
    controller.queue_task(pd_controller1);
    controller.queue_task(pd_controller2);
    controller.queue_task(writer);

    // execute the controller
    controller.execute(); 

    return 0;
}



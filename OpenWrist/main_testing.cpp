#include <iostream>
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include <functional>
#include "MelShare.h"

class PdController : public mel::Task {

public:
    PdController(OpenWrist& open_wrist, mel::Daq* daq) : Task("pd_controller"), open_wrist_(open_wrist), daq_(daq) {}

    OpenWrist open_wrist_;
    mel::Daq* daq_;

    //mel::share::MelShare map_ = mel::share::MelShare("ow_state");

    void start() override {
       
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
        getchar();
        daq_->activate();
        daq_->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist" << std::endl;
        getchar();
        open_wrist_.enable();
        std::cout << "Press Enter to start the controller" << std::endl;
        getchar();
        daq_->start_watchdog(0.1);
        std::cout << "Starting the controller ... " << std::endl;
    }

    void step() override {

        daq_->read_all();
        daq_->reload_watchdog();

        double traj0 = mel::sin_trajectory(80 * mel::DEG2RAD, 0.25, time());
        double traj1 = mel::sin_trajectory(60 * mel::DEG2RAD, 0.25, time());
        double traj2 = mel::sin_trajectory(30 * mel::DEG2RAD, 0.25, time());

        double torque0 = mel::pd_control_effort(25, 1.15, traj0, open_wrist_.joints_[0]->get_position(), 0, open_wrist_.joints_[0]->get_velocity());
        double torque1 = mel::pd_control_effort(25, 1.15, traj1, open_wrist_.joints_[1]->get_position(), 0, open_wrist_.joints_[1]->get_velocity());
        double torque2 = mel::pd_control_effort(25, 1.15, traj2, open_wrist_.joints_[2]->get_position(), 0, open_wrist_.joints_[2]->get_velocity());

        open_wrist_.joints_[0]->set_torque(torque0);
        open_wrist_.joints_[1]->set_torque(torque1);
        open_wrist_.joints_[2]->set_torque(torque2);

        open_wrist_.update_state_map();

        daq_->write_all();

    }

    void stop() override {
        open_wrist_.disable();
        daq_->deactivate();
    }

};


int main(int argc, char * argv[]) {  
    
    /*
    // set Windows thread priority
    // https://msdn.microsoft.com/en-us/library/windows/desktop/ms685100(v=vs.85).aspx
    HANDLE hThread = GetCurrentThread();
    SetPriorityClass(hThread, HIGH_PRIORITY_CLASS); // use REALTIME_PRIORITY_CLASS with extreme care!
    SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL);
    */

    
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

    // mel::Exo* open_wrist = new OpenWrist(config);
    OpenWrist open_wrist(config);

    // make a new Clock and Controller
    mel::Clock clock(1000, true); // 1000 Hz, clock logging enabled
    mel::Controller controller(clock);
    
    // queue Tasks for the Controller to execute
    controller.queue_task(new PdController(open_wrist, q8));

    // execute the controller
    controller.execute(); 
    
    return 0;
}



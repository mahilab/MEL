#include <iostream>
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include <functional>
#include "MelShare.h"
#include "DataLog.h"

class PdController : public mel::Task {

public:
    PdController(OpenWrist& open_wrist, mel::Daq* daq) : Task("pd_controller"), open_wrist_(open_wrist), daq_(daq) {}

    OpenWrist open_wrist_;
    mel::Daq* daq_;
    mel::DataLog my_log_ = mel::DataLog("my_log");

    void start() override {

        my_log_.add_col("Traj0").add_col("Traj1").add_col("Traj2");
       
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">.";
        getchar();
        daq_->activate();
        daq_->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        open_wrist_.enable();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq_->start_watchdog(0.5);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    }

    void step() override {

        daq_->read_all();
        daq_->reload_watchdog();

        double traj0 = mel::sin_trajectory(80 * mel::DEG2RAD, 0.25, time());
        double traj1 = mel::sin_trajectory(60 * mel::DEG2RAD, 0.25, time());
        double traj2 = mel::sin_trajectory(30 * mel::DEG2RAD, 0.25, time());

        std::vector<double> traj012 = { traj0, traj1, traj2 };

        my_log_.add_row(traj012);

        double torque0 = mel::pd_controller(25, 1.15, traj0, open_wrist_.joints_[0]->get_position(), 0, open_wrist_.joints_[0]->get_velocity());
        double torque1 = mel::pd_controller(25, 1.15, traj1, open_wrist_.joints_[1]->get_position(), 0, open_wrist_.joints_[1]->get_velocity());
        double torque2 = mel::pd_controller(25, 1.15, traj2, open_wrist_.joints_[2]->get_position(), 0, open_wrist_.joints_[2]->get_velocity());

        open_wrist_.joints_[0]->set_torque(torque0);
        open_wrist_.joints_[1]->set_torque(torque1);
        open_wrist_.joints_[2]->set_torque(torque2);

        open_wrist_.update_state_map();

        daq_->write_all();

    }

    void stop() override {
        open_wrist_.disable();
        daq_->deactivate();
        my_log_.save_data("testing");
    }

};


class Compensation : public mel::Task {

public:

    Compensation(OpenWrist& open_wrist, mel::Daq* daq) : Task("gravity_compensation"), ow_(open_wrist), daq_(daq) {}

    OpenWrist ow_;
    mel::Daq* daq_;

    void start() override {


        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">.";
        getchar();
        daq_->activate();
        daq_->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow_.enable();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq_->start_watchdog(0.5);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    }

    void step() override {

        daq_->read_all();
        daq_->reload_watchdog();

        ow_.joints_[0]->set_torque( ow_.compute_gravity_compensation(0) + ow_.compute_friction_compensation(0) );
        ow_.joints_[1]->set_torque( ow_.compute_gravity_compensation(1) + ow_.compute_friction_compensation(1) );
        ow_.joints_[2]->set_torque( ow_.compute_friction_compensation(2) * 0.5 );

        ow_.update_state_map();
        daq_->write_all();

    }

    void stop() override {
        ow_.disable();
        daq_->deactivate();
    }

};

class Calibrate : public mel::Task {

public:

    Calibrate(OpenWrist& open_wrist, mel::Daq* daq) : Task("ow_calibration"), ow(open_wrist), daq(daq) {}
    OpenWrist ow;
    mel::Daq* daq;

    double pos_ref;
    double pos_act;
    double vel_act;
    double vel_ref = 30 * mel::DEG2RAD;

    mel::share::MelShare scope = mel::share::MelShare("scope", 16);

    void start() override {
        std::cout << "Press ENTER to activate Daq <" << daq->name_ << ">.";
        getchar();
        daq->activate();
        daq->zero_encoders();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow.enable();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq->start_watchdog(0.1);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
        pos_ref = ow.joints_[1]->get_position();
    }

    void step() override {
        daq->read_all();
        daq->reload_watchdog();

        double pos_act = ow.joints_[1]->get_position();
        double vel_act = ow.joints_[1]->get_velocity();

        pos_ref += vel_ref * delta_time();

        double torque = mel::pd_controller(20, 1, pos_ref, pos_act, 0, vel_act);
        torque = mel::saturate(torque, 0.25);
        mel::double_vec scope_data = { time(),torque };


        ow.joints_[0]->set_torque(mel::pd_controller(25, 1.15, 0, ow.joints_[0]->get_position(), 0, ow.joints_[0]->get_velocity()));
        ow.joints_[1]->set_torque(torque);
        ow.joints_[2]->set_torque(mel::pd_controller(20, 0.25, 0, ow.joints_[2]->get_position(), 0, ow.joints_[2]->get_velocity()));

        scope.write(scope_data);

        ow.update_state_map();
        daq->write_all();
    }

    void stop() override {
        ow.disable();
        daq->deactivate();
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
    mel::Clock clock(1000, true);
    mel::Controller controller(clock);
    
    // queue Tasks for the Controller to execute
    mel::Task* task = new Calibrate(open_wrist, q8);
    controller.queue_task(task);

    // execute the controller
    controller.execute(); 

    // clean up
    delete q8;
    delete task;

    return 0;
}



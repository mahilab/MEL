#include <iostream>
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "Encoder.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include <functional>
#include "MelShare.h"
#include "DataLog.h"
#include <boost/program_options.hpp>
#include <noise/noise.h>
#include "Integrator.h"

namespace po = boost::program_options;

class MelScopeTest : public mel::Task {

public:

    MelScopeTest() : Task("scope_test") {};

    mel::share::MelShare cpp2py = mel::share::MelShare("cpp2py");
    mel::share::MelShare py2cpp = mel::share::MelShare("py2cpp");
    mel::share::MelShare int_map = mel::share::MelShare("int_map");

    std::array<double, 5> data_r = { 0.25, 0.25, 0.25, 0.25, 0.25 };
    std::array<double, 5> data_w = { 0, 0, 0, 0, 0 };

    std::array<double, 3> data_int = { 0,0,0 };

    Integrator integrator = Integrator(0);

    noise::module::Perlin perlin_module;

    void start() override {
        py2cpp.write(data_r);
    }

    void step() override {

        py2cpp.read(data_r);
        data_w[0] = mel::sin_wave(10, data_r[0], time());
        data_w[1] = 10 * perlin_module.GetValue(time(), 0, 0);
        data_w[2] = mel::square_wave(10, data_r[2], time());;
        data_w[3] = mel::triangle_wave(10, data_r[3], time());
        data_w[4] = mel::sawtooth_wave(10, data_r[4], time());
        cpp2py.write(data_w);

        data_int[0] = 2 * cos(2 * time());
        data_int[1] = sin(2 * time());
        data_int[2] = 0;
        int_map.write(data_int);

    }

    void stop() override {}

};


class CuffTest : public mel::Task {
public:

    CuffTest(OpenWrist* open_wrist, mel::Daq* daq) : Task("cuff_test"), ow_(open_wrist), daq_(daq) {}
    mel::Daq* daq_;

    OpenWrist* ow_;
    Cuff cuff_;

    double radius = 30 * mel::DEG2RAD;

    noise::module::Perlin perlin_module;

    void start() override {
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">.";
        getchar();
        daq_->activate();
        std::cout << "Press ENTER to enable OpenWrist.";
        getchar();
        ow_->enable();
        std::cout << "Press ENTER to enable CUFF";
        getchar();
        cuff_.enable();
        std::cout << "Press ENTER to start the controller.";
        getchar();
        daq_->start_watchdog(0.1);
        std::cout << "Executing the controller. Press CTRL+C to stop." << std::endl;
    }

    void step() override {
        daq_->reload_watchdog();
        daq_->read_all();
        ow_->update_state_map();

        double user_radius = sqrt(pow(ow_->joints_[1]->get_position(), 2) + pow(ow_->joints_[2]->get_position(), 2));
        double rad_error = radius - user_radius;

        double noise0 = 0.5 * perlin_module.GetValue(time(), 0, 0);
        double noise1 = 0.25 * perlin_module.GetValue(time(), 0, 0);

        ow_->joints_[0]->set_torque(noise0);
        ow_->joints_[1]->set_torque(noise1);


        double mot_input = mel::RAD2DEG * abs(rad_error) *500;
        cuff_.set_motor_positions((short int)(-mot_input), (short int)mot_input);

        daq_->write_all();
    }

    void stop() override {
        ow_->disable();
        daq_->deactivate();
        cuff_.disable();
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

    // set up program options 
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("calibrate", "calibrate OpenWrist zero position")
        ("transparency_mode", "puts OpenWrist in gravity and friction compensated state")
        ("testing","various testing")
        ("test", "quick tests")
        ("cuff_test", "Cuff testing biatch");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), var_map);
    boost::program_options::notify(var_map);

    if (var_map.count("help")) {
        mel::print(desc);
        return -1;
    }

    if (var_map.count("testing")) {
        mel::Clock clock(1000);
        mel::Controller controller(clock);
        mel::Task* task = new MelScopeTest();
        controller.queue_task(task);
        controller.execute();
        return 0;
    }

    if (var_map.count("test")) {
        const char* name_data = "mel";
        std::string name_temp = std::string(name_data) + "_size";
        const char* name_size = name_temp.c_str();
        std::cout << name_size << std::endl;
    }

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
    
    if (var_map.count("calibrate")) {
        open_wrist.calibrate(q8);
    }

    if (var_map.count("transparency_mode")) {
        open_wrist.transparency_mode(q8);
    }

    if (var_map.count("cuff_test")) {
        mel::Clock clock(1000);
        mel::Controller controller(clock);
        mel::Task* task = new CuffTest(&open_wrist, q8);
        controller.queue_task(task);
        controller.execute();
    }


    // clean up
    delete q8;

    return 0;
}



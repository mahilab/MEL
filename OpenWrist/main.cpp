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
#include <boost/program_options.hpp>

namespace po = boost::program_options;

class MelScopeTest : public mel::Task {

public:

    MelScopeTest() : Task("scope_test") {};

    mel::share::MelShare map0 = mel::share::MelShare("map0");
    mel::share::MelShare map1 = mel::share::MelShare("map1");

    std::array<double, 5> data0 = { 0, 0,0,0,0 };
    std::array<double, 1> data1 = { 0 };

    void start() override {}
    void step() override {

        for (int i = 0; i < 5; i++) {
            data0[i] = mel::sin_trajectory(1, 1.0/pow(10,i), time());
        }
        map0.write(data0);

        data1[0] = data0[1];
        map1.write(data1);
    }
    void stop() override {}

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
        ("test", "quick tests");

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


    // clean up
    delete q8;

    return 0;
}



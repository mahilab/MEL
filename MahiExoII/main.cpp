#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "ControlLoop.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;


class MyController : public mel::Controller {

public:

    MyController(mel::MahiExoII* exo) :
        exo_(exo)
    {
    
    }

    mel::MahiExoII* exo_;

    
    void start() override {
        std::cout << "Starting MyController" << std::endl;

        // initialize Q8 USB
        exo_->daq_->activate();
        exo_->daq_->start_watchdog(0.1);

        
    }

    void step() override {

        exo_->daq_->reload_watchdog();
        exo_->daq_->read_all();

        exo_->get_joint_positions();
        mel::print_double_vec(exo_->psi_);


    }

    void stop() override {
        std::cout << "Stopping MyController" << std::endl;

        exo_->daq_->deactivate();
    }
    void pause() override {
        std::cout << "Pausing MyController" << std::endl;
    }
    void resume() override {
        std::cout << "Resuming MyController" << std::endl;
    }
};

int main(int argc, char * argv[]) {


    /* set up program options */
    po::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("zero", "zeros encoder counts on startup");

    po::variables_map var_map;
    po::store(po::parse_command_line(argc, argv, desc), var_map);
    po::notify(var_map);

    
    if (var_map.count("help")) {
        std::cout << desc << "\n";
        return -1;
    }
    

    // instantiate Q8 USB for encoders, actuator controls, and EMG
    std::string id = "0";
    mel::uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::uint_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::uint_vec  di_channels = {};
    mel::uint_vec  do_channels = { 0, 1, 2, 3, 4 };
    mel::uint_vec enc_channels = { 0, 1, 2, 3, 4 };
    char options[] = "update_rate=fast;decimation=1";
    mel::Daq *q8_0 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);

    /*
    // instantiate Q8 USB for ATI and Futek force/torque sensors
    std::string id = "1";
    uint_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    uint_vec  ao_channels = {};
    uint_vec  di_channels = {};
    uint_vec  do_channels = {};
    uint_vec enc_channels = {};
    char options[] = "update_rate=fast;decimation=1";
    Daq *q8_1 = new Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);
    */

    // instantiate MahiExoII
    
    mel::MahiExoII exo = mel::MahiExoII(q8_0, ai_channels, ao_channels, di_channels, do_channels, enc_channels);
                              

    /* manual zero joint positions */
    if (var_map.count("zero")) {
        exo.daq_->activate();
        exo.daq_->zero_encoders();
    }
    

    // create controller and control loop and clock
    mel::Controller* my_controller = new MyController(&exo);
    mel::Clock clock(1000);
    mel::ControlLoop loop(clock);

    // queue controllrs
    loop.queue_controller(my_controller);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // execute the controller
    loop.execute();

    // delete controller
    delete my_controller;

    return 0;
}

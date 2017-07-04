#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "ControlLoop.h"
#include <boost/program_options.hpp>
#include "Exo.h"

namespace po = boost::program_options;

class MyTask : public mel::Controller {

public:

    mel::Daq* q8;
    mel::MahiExoII* exo_;

    void start() override {
        std::cout << "Starting MyController" << std::endl;

        // instantiate Q8 USB for encoders, actuator controls, and EMG
        std::string id = "0";
        mel::uint32_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
        mel::uint32_vec  ao_channels = { 0, 1, 2, 3, 4 };
        mel::uint32_vec  di_channels = {};
        mel::uint32_vec  do_channels = { 0, 1, 2, 3, 4 };
        mel::uint32_vec enc_channels = { 0, 1, 2, 3, 4 };
        char options[] = "update_rate=fast;decimation=1";
        q8 =  new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);
        
        /*
        do0 = q8->do_channel(0);
        ao0 = q8->ao_channel(0);
        enc0 = q8->encoder_channel(0);
        encr0 = q8->encrate_channel(0);
        
        do1 = q8->do_channel(1);
        ao1 = q8->ao_channel(1);
        enc1 = q8->encoder_channel(1);
        encr1 = q8->encrate_channel(1);

        do2 = q8->do_channel(2);
        ao2 = q8->ao_channel(2);
        enc2 = q8->encoder_channel(2);
        encr2 = q8->encrate_channel(2);

        do3 = q8->do_channel(3);
        ao3 = q8->ao_channel(3);
        enc3 = q8->encoder_channel(3);
        encr3 = q8->encrate_channel(3);

        do4 = q8->do_channel(4);
        ao4 = q8->ao_channel(4);
        enc4 = q8->encoder_channel(4);
        encr4 = q8->encrate_channel(4);
        */

        // instantiate MahiExoII
        exo_ = new mel::MahiExoII(q8);


        // initialize Q8 USB
        q8->activate();
        q8->start_watchdog(0.1);        
        
    }

    void step() override {

        q8->reload_watchdog();
        q8->read_all();
        
        //robot_->get_joint_positions;
        //robot_->get_joint_velocities;
        //robot_->set_joint_torques;
        q8->write_all();
    }

    void stop() override {
        std::cout << "Stopping MyController" << std::endl;

        q8->deactivate();
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


    /*
    // manual zero joint positions
    if (var_map.count("zero")) {
        robot.daqs_[0]->activate();
        robot.daqs_[0]->offset_encoder_counts({ 0, 0, 29125, 29125, 29125 });
    }
    */
    
    // create controller and control loop and clock
    mel::Controller* my_controller = new MyTask();
    mel::Clock clock(1000,true);
    mel::Controller loop(clock);

    // queue controllrs
    loop.queue_task(my_controller);

    // request users permission to execute the controller
    std::cout << "Press ENTER to execute the controller. CTRL+C will stop the controller once it's started." << std::endl;
    getchar();

    // execute the controller
    loop.execute();

    // delete controller
    delete my_controller;
  
    return 0;
}
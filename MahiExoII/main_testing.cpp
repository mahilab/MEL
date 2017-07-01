#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "Robot.h"
#include "Controller.h"
#include "ControlLoop.h"
#include <boost/program_options.hpp>
#include "Exo.h"

namespace po = boost::program_options;

class MyController : public mel::Controller {

public:

    std::vector<mel::Daq*> daqs_;
    mel::Robot* robot_;

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
        daqs_ = { new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options) };
        

        // instantiate generic robot
        
        robot_ = new mel::Robot();


        // initialize Q8 USB
        robot_->daqs_[0]->activate();
        robot_->daqs_[0]->start_watchdog(0.1);        
        
    }

    void step() override {

        robot_->daqs_[0]->reload_watchdog();
        robot_->daqs_[0]->read_all();
        
        //robot_->get_joint_positions;
        //robot_->get_joint_velocities;
        //robot_->set_joint_torques;
        robot_->daqs_[0]->write_all();
    }

    void stop() override {
        std::cout << "Stopping MyController" << std::endl;

        robot_->daqs_[0]->deactivate();
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
    mel::Controller* my_controller = new MyController();
    mel::Clock clock(1000,true);
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
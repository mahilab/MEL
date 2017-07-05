#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "Task.h"
#include <boost/program_options.hpp>
#include "Exo.h"

namespace po = boost::program_options;

class MyTask : public mel::Task {

public:

    MyTask(MahiExoII exo, mel::Daq* daq) : Task("testing"), exo_(exo), daq_(daq) {}

    MahiExoII exo_;
    mel::Daq* daq_;

    void start() override {
        std::cout << "Press ENTER to activate Daq <" << daq_->name_ << ">" << std::endl;
        getchar();
        daq_->activate();
        std::cout << "Press ENTER to enable MahiExoII" << std::endl;
        getchar();
        exo_.enable();
        std::cout << "Press Enter to start the controller" << std::endl;
        getchar();
        daq_->start_watchdog(0.1);
        std::cout << "Starting the controller ... " << std::endl;
        
    }

    void step() override {

        daq_->read_all();
        daq_->reload_watchdog();
        
        
       /* std::cout << exo_.joints_[0]->get_position() << " "
                  << exo_.joints_[1]->get_position() << " "
                  << exo_.joints_[2]->get_position() << " " 
                  << exo_.joints_[3]->get_position() << " " 
                  << exo_.joints_[4]->get_position() << std::endl;
                  */

        mel::print_double_vec(exo_.get_joint_positions());

        //robot_->get_joint_velocities();
        //robot_->set_joint_torques();
        



        //daq_->write_all();
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

    //  create a Q8Usb object
    mel::uint32 id = 0;

    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec  di_channels = { };
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };
    
    mel::Daq* q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

    // create and configure an MahiExoII object
    MahiExoII::Config config;
    for (int i = 0; i < 5; i++) {
        config.enable_[i] = q8->do_(i);
        config.command_[i] = q8->ao_(i);
        config.encoder_[i] = q8->encoder_(i);
        config.encrate_[i] = q8->encrate_(i);
        config.amp_gains_[i] = 1;
    }
    MahiExoII exo(config);


    // manual zero joint positions
    if (var_map.count("zero")) {
        q8->activate();
        q8->offset_encoders({ 0, 0, 29125, 29125, 29125 });
    }
    
    
    // make a new Clock and Controller
    mel::Clock clock(1000, true); // 1000 Hz, clock logging enabled
    mel::Controller controller(clock);

    // queue Tasks for the Controller to execute
    controller.queue_task(new MyTask(exo, q8));

    // execute the controller
    controller.execute();
  
    return 0;
}
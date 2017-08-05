#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "util.h"
#include "MahiExoII.h"
#include "Controller.h"
#include "Task.h"
#include <boost/program_options.hpp>
#include "Exo.h"
#include "EmgTraining.h"


namespace po = boost::program_options;


int main(int argc, char * argv[]) {

    // set up program options 
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
    mel::channel_vec  di_channels = {};
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };

    mel::Q8Usb::Options options;
    for (int i = 0; i < 8; i++) {
        options.do_initial_signals_[i] = 1;
        options.do_final_signals_[i] = 1;
        options.do_expire_signals_[i] = 1;
    }

    mel::Daq* q8 = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    // create and configure a MahiExoII object
    mel::double_vec amp_gains = { 1.8, 1.8, 0.184, 0.184, 0.184 };
    MahiExoII::Config config;
    for (int i = 0; i < 5; i++) {
        config.enable_[i] = q8->do_(i);
        config.command_[i] = q8->ao_(i);
        config.encoder_[i] = q8->encoder_(i);
        config.encrate_[i] = q8->encrate_(i);
        config.amp_gains_[i] = amp_gains[i];
    }
    MahiExoII* mahiexoii = new MahiExoII(config);

    // manual zero joint positions
    if (var_map.count("zero")) {
        q8->activate();
        q8->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        q8->deactivate();
        return 0;
    }

    mel::Clock clock(1000, false);

    EmgTraining sm = EmgTraining(clock,q8,mahiexoii);

    sm.execute(5);

    delete q8;

    return 0;

}
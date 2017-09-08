#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "Clock.h"
#include "MahiExoII.h"
#include "util.h"
#include "mahiexoii_util.h"
#include <boost/program_options.hpp>
#include "TransparentMode.h"
#include "SmoothPositionControl.h"
#include "MelShare.h"
#include "GuiFlag.h"
#include "Input.h"



int main(int argc, char * argv[]) {

    // ignore CTRL-C signal (we can do this with Input)
    signal(SIGINT, SIG_IGN);

    // set up program options 
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("zero", "zeros encoder counts on startup");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), var_map);
    boost::program_options::notify(var_map);

    if (var_map.count("help")) {
        mel::print(desc);
        return 0;
    }

    //  create a Q8Usb object for the board connected to robot motors
    mel::uint32 id_mot = 0;
    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 1, 2, 3, 4, 5 };
    mel::channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec enc_channels = { 1, 2, 3, 4, 5 };
    mel::Q8Usb::Options options;
    for (int i = 0; i < 8; ++i) {
        options.do_initial_signals_[i] = 1;
        options.do_final_signals_[i] = 1;
        options.do_expire_signals_[i] = 1;
    }
    mel::Daq* q8_mot = new mel::Q8Usb(id_mot, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    // create and configure a MahiExoII object
    mel::MahiExoII::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_mot->do_(i + 1);
        config.command_[i] = q8_mot->ao_(i + 1);
        config.encoder_[i] = q8_mot->encoder_(i + 1);
        config.encrate_[i] = q8_mot->encrate_(i + 1);
    }
    mel::MahiExoII meii(config);

    // manual zero joint positions
    if (var_map.count("zero")) {
        q8_mot->enable();
        q8_mot->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        q8_mot->disable();
        return 0;
    }

    // run transparent mode
    mel::Clock clock(1000);
    TransparentMode tp_mode(clock, q8_mot, meii);
    tp_mode.execute();
    delete q8_mot;
    return 0;

}
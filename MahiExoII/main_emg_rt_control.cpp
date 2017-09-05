#include <iostream>
#include <csignal>
#include "Q8Usb.h"
#include "Clock.h"
#include "MahiExoIIEmg.h"
#include "util.h"
#include "mahiexoii_util.h"
#include <boost/program_options.hpp>
#include "EmgRTControl.h"
#include "MelShare.h"
#include "GuiFlag.h"


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

    //  create a Q8Usb object
    mel::uint32 id_emg = 0;
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
    mel::Daq* q8_emg = new mel::Q8Usb(id_emg, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    // create and configure a MahiExoII object
    MahiExoIIEmg::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_emg->do_(i+1);
        config.command_[i] = q8_emg->ao_(i+1);
        config.encoder_[i] = q8_emg->encoder_(i+1);
        config.encrate_[i] = q8_emg->encrate_(i+1);
    }
    for (int i = 0; i < 8; ++i) {
        config.emg_[i] = q8_emg->ai_(i);
    }
    MahiExoIIEmg meii(config);

    // manual zero joint positions
    if (var_map.count("zero")) {
        q8_emg->enable();
        q8_emg->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        q8_emg->disable();
        return 0;
    }

    // create GUI flag
    GuiFlag gui_flag("gui_flag", 0);


    // run the experiment
    int input_mode = 0;
    mel::Clock clock(1000);
    EmgRTControl emg_rt_control(clock, q8_emg, meii, gui_flag, input_mode);
    emg_rt_control.execute();
    delete q8_emg;
    return 0;

}
#include <iostream>
#include "Task.h"
#include "Controller.h"
#include "Clock.h"
#include "Q8Usb.h"
#include "OpenWrist.h"
#include "Cuff.h"
#include "MelShare.h"
#include "DataLog.h"
#include <boost/program_options.hpp>
#include "HGCondition0.h"
#include "GuiFlag.h"
#include "HapticGuidance.h"

int main(int argc, char * argv[]) {

    // set up program options 
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("calibrate", "calibrate OpenWrist zero position")
        ("condition0", "pendulum simulation");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), var_map);
    boost::program_options::notify(var_map);

    if (var_map.count("help")) {
        mel::print(desc);
        return -1;
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

    // perform calibrate command if requested by user
    if (var_map.count("calibrate")) {
        open_wrist.calibrate(q8);
        return 0;
    }

    // create GUI flag
    GuiFlag gui_flag("gui_flag", 0);

    if (var_map.count("condition0")) {
        mel::Clock clock(1000, true);
        HapticGuidance haptic_guidance(clock, &open_wrist, q8, gui_flag);
        haptic_guidance.execute();
        return 0;
    }

}
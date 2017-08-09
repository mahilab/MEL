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
        ("run", "run the haptic guidance experiment")
        ("input-mode", boost::program_options::value<int>(), "0 = Terminal, 1 = GUI")
        ("subject-num", boost::program_options::value<int>(), "the subject number 1-40")
        ("condition", boost::program_options::value<int>(), "0 = OW w/ PN, 1 = OW+CUFF w/ PN, 2 = OW+CUFF w/ HG, 3 = OW+MEII w/ HG")
        ("task", boost::program_options::value<int>(), "0 = familirization, 1 = evaluation, 2 = training, 3 = break, 4 = generalization")
        ("task-num", boost::program_options::value<int>(), "the task number")
        ("trial-num", boost::program_options::value<int>(), "the trial number within the task number");


    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), var_map);
    boost::program_options::notify(var_map);

    if (var_map.count("help")) {
        mel::print(desc);
        return 0;
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

    // run the experiment
    if (var_map.count("run")) {
        if (var_map.count("subject-num") && var_map.count("condition") && var_map.count("task") && var_map.count("task-num") && var_map.count("trial-num")) {

        int input_mode = 0;
        if (var_map.count("input-mode") && var_map["input-mode"].as<int>() == 1)
            input_mode = 1;

        mel::Clock clock(1000);
        HapticGuidance haptic_guidance(clock, &open_wrist, q8, gui_flag, var_map["subject-num"].as<int>(), var_map["condition"].as<int>(), input_mode);
        haptic_guidance.execute();

        return 0;
        }
        else {
            mel::print("The 'run' command was supplied but either 'subject-num', 'condition', 'task', 'task-num', or 'trial-num' were not. Try again.");
        }
    }


}
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
        ("subject-num", boost::program_options::value<int>(), "the subject number, 1-40")
        ("condition", boost::program_options::value<int>(), "1 = OW w/ PN, 2 = OW+CUFF w/ PN, 3 = OW+CUFF w/ HG, 4 = OW+MEII w/ HG")
        ("start-trial", boost::program_options::value<std::string>(), "the trial to start at, e.g. F1-1, T3-5, G2-12, etc");

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

    // create and configure CUFF object
    Cuff cuff;

    // perform calibrate command if requested by user
    if (var_map.count("calibrate")) {
        open_wrist.calibrate(q8);
        return 0;
    }

    // create GUI flag
    GuiFlag gui_flag("gui_flag", 0);

    // create MELShare for GUI start commmands
    mel::share::MelShare start_commands("start_commands");
    std::array<int, 5> start_commands_data;

    int subject, condition;
    std::string start_trial;

    // check input mode
    int input_mode;
    if (var_map.count("subject-num") && var_map.count("condition") && var_map.count("start-trial")) {
        input_mode = 0;
        subject = var_map["subject-num"].as<int>();
        condition = var_map["condition"].as<int>();
        start_trial = var_map["start-trial"].as<std::string>();
    }
    else if (var_map.count("subject-num") && var_map.count("condition")) {
        input_mode = 0;
        subject = var_map["subject-num"].as<int>();
        condition = var_map["condition"].as<int>();
        start_trial = "F1-1";
    }
    else if (var_map.count("input-mode") && var_map["input-mode"].as<int>() == 1) {
        system("start GUI.pyw &");
        input_mode = 1;
        mel::print("Wait for input from GUI");
        gui_flag.wait_for_flag(1);
        gui_flag.reset_flag(0);
        start_commands.read(start_commands_data);
        subject = start_commands_data[0];
        condition = start_commands_data[1];
        start_trial = "F1-1"; // TODO
    }
    else {
        mel::print("Not enough input parameters were provided to run the experiment.");
        return -1;
    }

    // run the experiment
    if (var_map.count("run")) {
        mel::print("");
        mel::print("RUNNING EXPERIMENT WITH THE FOLLOWING INPUTS:");
        mel::print("Subject Number: " + std::to_string(subject));
        mel::print("Condition:      " + std::to_string(condition));
        mel::print("Start Trial:    " + start_trial);
        mel::print("");

        mel::Clock clock(1000);
        HapticGuidance haptic_guidance(clock, q8, open_wrist, cuff, gui_flag, input_mode, subject, condition, start_trial);
        haptic_guidance.execute();
        return 0;
    }
}
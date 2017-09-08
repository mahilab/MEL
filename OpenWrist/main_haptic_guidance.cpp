#include <iostream>
#include "Clock.h"
#include "Q8Usb.h"
#include "OpenWrist.h"
#include "MahiExoII.h"
#include "Cuff.h"
#include "MelShare.h"
#include "DataLog.h"
#include <boost/program_options.hpp>
#include "GuiFlag.h"
#include "HapticGuidance.h"
#include <csignal>

int main(int argc, char * argv[]) {

    // ignore CTRL-C signal (we can do this with Input)
    signal(SIGINT, SIG_IGN);

    // enable soft realtime
    mel::enable_soft_realtime();

    // set up program options 
    boost::program_options::options_description desc("Available Options");
    desc.add_options()
        ("help", "produces help message")
        ("calibrate-ow", "calibrate OpenWrist zero position (auto)")
        ("calibrate-meii", "calibrate MahiExo-II zero position (manual)")
        ("transparent", "puts the OpenWrist in transparency mode indefinitely")
        ("run", "run the haptic guidance experiment")
        ("input", boost::program_options::value<int>(), "0 = Terminal, 1 = GUI")
        ("subject", boost::program_options::value<int>(), "the subject number, 1-40")
        ("condition", boost::program_options::value<int>(), "1 = OW w/ PN, 2 = OW+CUFF w/ PN, 3 = OW+CUFF w/ HG, 4 = OW+MEII w/ HG")
        ("trial", boost::program_options::value<std::string>(), "the trial to start at, e.g. F1-1, T3-5, G2-12, etc")
        ("meii", "append if MahiExo-II is present");

    boost::program_options::variables_map var_map;
    boost::program_options::store(boost::program_options::command_line_parser(argc, argv).options(desc).allow_unregistered().run(), var_map);
    boost::program_options::notify(var_map);

    if (var_map.count("help")) {
        mel::print(desc);
        return 0;
    }

    // identify Q8Usb's
    mel::uint32 id_ow = 0;

    /*
    mel::uint32 id_meii = 1;
    if ((var_map.count("condition") && var_map["condition"].as<int>() == 4) || var_map.count("calibrate-meii")) {
        id_ow = 1;
        id_meii = 0;
        if (!mel::Q8Usb::check_digital_loopback(id_meii, 7)) {
            mel::print("Warning: Digital loopback not connected to Q8Usb 0");
            if (mel::Q8Usb::check_digital_loopback(id_ow, 7)) {
                id_ow = 0;
                id_meii = 1;
            }
            else {
                mel::print("Error: Digital loopback not connected to Q8Usb 1. EMG DAQ not identified.");
                return -1;
            }
        }
    }
    */

    // create Q8Usb OpenWrist
    mel::channel_vec  ai_channels = { 0, 1, 2 };
    mel::channel_vec  ao_channels = { 0, 1, 2,3 };
    mel::channel_vec  di_channels = { 0, 1, 2 };
    mel::channel_vec  do_channels = { 0, 1, 2 };
    mel::channel_vec enc_channels = { 0, 1, 2 };

    mel::Q8Usb::Options options_q8;
    options_q8.update_rate_ = mel::Q8Usb::Options::UpdateRate::Fast_8kHz;
    options_q8.decimation_ = 1;
    options_q8.ao_modes_[0] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options_q8.ao_modes_[1] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0, -1.382, 8.030, 0, -1, 0, 1000);
    options_q8.ao_modes_[2] = mel::Q8Usb::Options::AoMode(mel::Q8Usb::Options::AoMode::CurrentMode1, 0,  1.912, 18.43, 0, -1, 0, 1000);

    mel::Daq* q8_ow = new mel::Q8Usb(id_ow, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options_q8);

    /*
    // create Q8Usb MahiExo-II
    ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    ao_channels = { 1, 2, 3, 4, 5 };
    di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    enc_channels = { 1, 2, 3, 4, 5 };
    mel::Q8Usb::Options options_meii;
    for (int i = 0; i < 8; ++i) {
        options_meii.do_initial_signals_[i] = 1;
        options_meii.do_final_signals_[i] = 1;
        options_meii.do_expire_signals_[i] = 1;
    }
    mel::Daq* q8_meii = new mel::Q8Usb(id_meii, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options_meii);    

    */

    // create and configure an OpenWrist object
    mel::OpenWrist::Config ow_config;
    for (int i = 0; i < 3; i++) {
        ow_config.enable_[i] = q8_ow->do_(i);
        ow_config.command_[i] = q8_ow->ao_(i);
        ow_config.sense_[i] = q8_ow->ai_(i);
        ow_config.encoder_[i] = q8_ow->encoder_(i);
        ow_config.encrate_[i] = q8_ow->encrate_(i);
        ow_config.amp_gains_[i] = 1;
    }
    mel::OpenWrist open_wrist(ow_config);
    
    /*
    // create and configure a MahiExo-II object
    mel::MahiExoII::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] =  q8_meii->do_(i + 1);
        config.command_[i] = q8_meii->ao_(i + 1);
        config.encoder_[i] = q8_meii->encoder_(i + 1);
        config.encrate_[i] = q8_meii->encrate_(i + 1);
    }
    mel::MahiExoII meii(config);    
    */

    // create and configure CUFF object
    Cuff cuff("cuff_forearm");    

    // perform calibration commands if requested by user
    if (var_map.count("calibrate-ow")) {
        // mel::print(static_cast<mel::Q8Usb*>(q8_ow)->options_.ao_final_voltages_);
        open_wrist.calibrate();
        //q8_ow->enable();
        //q8_ow->ao_(0).set_voltage(0);
        //q8_ow->write_all();
        //getchar();
        delete q8_ow;
        //delete q8_meii;
        return 0;
    }
    /*
    if (var_map.count("calibrate-meii")) {
        q8_meii->enable();
        q8_meii->offset_encoders({ 0, -33259, 29125, 29125, 29125 });
        q8_meii->disable();
        delete q8_ow;
        delete q8_meii;
        return 0;
    }
    */

    // put the OpenWrist in transparency mode if requested by user
    if (var_map.count("transparent")) {
        open_wrist.transparency_mode();
        delete q8_ow;
       // delete q8_meii;
        return 0;
    }

    // create GUI flag
    GuiFlag gui_flag("gui_flag", 0);

    // create MELShare for GUI start commmands
    mel::share::MelShare start_commands("start_commands");
    std::array<int, 5> start_commands_data;

    int subject, condition, input_mode;
    std::string start_trial;

    if (var_map.count("run")) {
        if (var_map.count("subject") && var_map.count("condition") && var_map.count("trial")) {
            input_mode = 0;
            subject = var_map["subject"].as<int>();
            condition = var_map["condition"].as<int>();
            start_trial = var_map["trial"].as<std::string>();
        }
        else if (var_map.count("subject") && var_map.count("condition")) {
            input_mode = 0;
            subject = var_map["subject"].as<int>();
            condition = var_map["condition"].as<int>();
            start_trial = "F1-1";
        }
        else if (var_map.count("input") && var_map["input"].as<int>() == 1) {
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
            delete q8_ow;
            //delete q8_meii;
            return -1;
        }

        // run the experiment
        mel::print("\nSubject Number: " + std::to_string(subject));
        mel::print("Condition:      " + std::to_string(condition));
        mel::print("Start Trial:    " + start_trial);

        mel::Clock clock(1000);
        //HapticGuidance haptic_guidance(clock, q8_ow, open_wrist, q8_meii, meii, cuff, gui_flag, input_mode, subject, condition, start_trial);
        //haptic_guidance.execute();
        delete q8_ow;
        //delete q8_meii;
        return 0;
    }    

}
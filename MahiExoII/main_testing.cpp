#include <iostream>
#include <csignal>
#include "MahiExoIIEmgFrc.h"



int main(int argc, char * argv[]) {

    mel::uint32 id_emg = 0;
    mel::uint32 id_ati = 1;
    if (!check_digital_loopback(0, 7)) {
        mel::print("Warning: Digital loopback not connected to Q8Usb 0");
        id_emg = 1;
        id_ati = 0;
    }

    //  create a Q8Usb object
    mel::channel_vec  ai_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  ao_channels = { 0, 1, 2, 3, 4 };
    mel::channel_vec  di_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec  do_channels = { 0, 1, 2, 3, 4, 5, 6, 7 };
    mel::channel_vec enc_channels = { 0, 1, 2, 3, 4 };

    mel::Q8Usb::Options options;
    for (int i = 0; i < 8; ++i) {
        options.do_initial_signals_[i] = 1;
        options.do_final_signals_[i] = 1;
        options.do_expire_signals_[i] = 1;
    }

    mel::Daq* q8_emg = new mel::Q8Usb(id_emg, ai_channels, ao_channels, di_channels, do_channels, enc_channels, options);


    //  create a second Q8Usb object
    ai_channels = { 0, 1, 2, 3, 4, 5 };
    ao_channels = {};
    di_channels = {};
    do_channels = {};
    enc_channels = {};

    mel::Daq* q8_ati = new mel::Q8Usb(id_ati, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

    MahiExoIIEmgFrc::Config config;
    for (int i = 0; i < 5; ++i) {
        config.enable_[i] = q8_emg->do_(i);
        config.command_[i] = q8_emg->ao_(i);
        config.encoder_[i] = q8_emg->encoder_(i);
        config.encrate_[i] = q8_emg->encrate_(i);
    }
    for (int i = 0; i < 8; ++i) {
        config.emg_[i] = q8_emg->ai_(i);
    }
    for (int i = 0; i < 6; ++i) {
        config.wrist_force_sensor_[i] = q8_ati->ai_(i);
    }
    MahiExoIIEmgFrc* meii = new MahiExoIIEmgFrc(config);

    return 0;

}
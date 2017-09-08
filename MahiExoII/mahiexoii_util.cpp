#include "mahiexoii_util.h"

bool check_digital_loopback(mel::uint32 daq_id, mel::channel digital_channel) {

    
    //  create a Q8Usb object
    mel::uint32 id = daq_id;
    mel::channel_vec  ai_channels = { };
    mel::channel_vec  ao_channels = { };
    mel::channel_vec  di_channels = { digital_channel };
    mel::channel_vec  do_channels = { digital_channel };
    mel::channel_vec enc_channels = { };
    mel::Daq* q8_temp = new mel::Q8Usb(id, ai_channels, ao_channels, di_channels, do_channels, enc_channels);

    // create clock
    mel::Clock clock(100);

    mel::dsignal di_signal = 0;
    mel::dsignal do_signal = 0;
    bool is_connected = true;

    q8_temp->enable();
    q8_temp->start_watchdog(0.1);

    clock.start();

    for (auto i = 0; i < 11; i++) {

        q8_temp->reload_watchdog();

        if (i > 0) {
            q8_temp->read_digitals();
            di_signal = q8_temp->get_digital_signal(digital_channel);
            if (di_signal != do_signal) {
                is_connected = false;
            }
            do_signal = 1 - do_signal;
        }

        q8_temp->set_digital_signal(digital_channel, do_signal);
        q8_temp->write_digitals();

        clock.wait();
    }

    delete q8_temp;
    q8_temp = nullptr;

    return is_connected;

}

double moving_set_point(double init_pos, double goal_pos, double init_time, double time, double speed) { 

    double ref_pos = init_pos + (goal_pos - init_pos) * mel::saturate((time - init_time) * speed / abs(goal_pos - init_pos), 1, 0);

    return ref_pos;

}


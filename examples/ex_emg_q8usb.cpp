#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Math/Functions.hpp>
#include <vector>
#include <sstream>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}


int main(int argc, char *argv[]) {

    // handle inputs 
    std::vector<uint32> emg_channel_numbers;
    if (argc > 1) {
        uint32 ch;
        for (int i = 1; i < argc; ++i) {
            std::stringstream ss(argv[i]);
            ss >> ch;
            if (ch >= 0 & ch < 8) {
                emg_channel_numbers.push_back(ch);
            }          
        }
    }
    else {
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_handler(handler);

    // initialize logger
    init_logger();

    // make MelShares
    MelShare ms_mes_raw("ms_mes_raw");
    MelShare ms_mes_demean("ms_mes_demean");
    MelShare ms_mes_env("ms_mes_env");
    MelShare ms_mes_tkeo_env("ms_mes_tkeo_env");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure
    Q8Usb q8(QOptions(), true, true, emg_channel_numbers);
    emg_channel_numbers = q8.analog_input.get_channel_numbers();
    std::size_t emg_channel_count = q8.analog_input.get_channel_count(); // number of EMG channels
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High)); // set this way for MEII
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High)); // set this way for MEII
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High)); // set this way for MEII

    // make EMG Electrodes
    size_t mes_buffer_size = 500;
    size_t mes_feature_window_size = 200;
    std::vector<EmgElectrode> emg_electrodes;
    for (size_t i = 0; i < emg_channel_count; ++i) {
        emg_electrodes.emplace_back(q8.analog_input[emg_channel_numbers[i]]);
        emg_electrodes[i].resize_mes_buffer(mes_buffer_size);
    }

    // list of feature names
    std::vector<std::string> feat_names = { "RMS", "MAV", "WL", "ZC", "SSC", "AR1", "AR2", "AR3", "AR4" };

    // create data logger for MES data
    DataLogger emg_data_logger(WriterType::Buffered, false);
    std::vector<std::string> emg_data_header;
    for (int i = 0; i < emg_channel_count; ++i) {
        emg_data_header.push_back("MES RAW " + stringify(emg_channel_numbers[i]));
    }
    for (int i = 0; i < emg_channel_count; ++i) {
        emg_data_header.push_back("MES DM " + stringify(emg_channel_numbers[i]));
    }
    for (int i = 0; i < emg_channel_count; ++i) {
        emg_data_header.push_back("MES ENV " + stringify(emg_channel_numbers[i]));
    }
    for (int i = 0; i < emg_channel_count; ++i) {
        emg_data_header.push_back("MES TKEO ENV " + stringify(emg_channel_numbers[i]));
    }
    emg_data_logger.set_header(emg_data_header);

    
    // create local variables
    std::vector<double> mes_raw(emg_channel_count);
    std::vector<double> mes_demean(emg_channel_count);
    std::vector<double> mes_env(emg_channel_count);
    std::vector<double> mes_tkeo_env(emg_channel_count);
    std::vector<double> mes_tkeo_env_mean(emg_channel_count);
    std::vector<double> mes_log_row(4 * emg_channel_count);
    bool mes_buffers_full = false;

    // write to MelShares
    ms_mes_raw.write_data(mes_raw);
    ms_mes_demean.write_data(mes_demean);
    ms_mes_env.write_data(mes_env);
    ms_mes_tkeo_env.write_data(mes_tkeo_env);

    // enable DAQ
    q8.enable();

    // construct timer in hybrid mode to avoid using 100% CPU
    Timer timer(milliseconds(1), Timer::Hybrid);

    // construct clock to regulate feature computation
    Clock feature_refract_clock;
    Time feature_refract_time = seconds(1);

    // wait for user input
    prompt("Press Enter to continue.");
    print("Reading EMG. Press Enter again to compute features. Press Escape to exit program.");
    sleep(seconds(1));

    // start loop
    q8.watchdog.start();
    while (!stop) {

        // update all DAQ input channels
        q8.update_input();

        // run signal processing on MES
        for (size_t i = 0; i < emg_channel_count; ++i) {
            emg_electrodes[i].update_and_buffer();
            mes_raw[i] = emg_electrodes[i].get_mes_raw();
            mes_demean[i] = emg_electrodes[i].get_mes_demean();
            mes_env[i] = emg_electrodes[i].get_mes_envelope();
            mes_tkeo_env[i] = emg_electrodes[i].get_mes_tkeo_envelope();
        }

        // write to MelShares
        ms_mes_raw.write_data(mes_raw);
        ms_mes_demean.write_data(mes_demean);
        ms_mes_env.write_data(mes_env);
        ms_mes_tkeo_env.write_data(mes_tkeo_env);

        // store data to data logger
        auto it = std::copy(mes_raw.begin(), mes_raw.end(), mes_log_row.begin());
        it = std::copy(mes_demean.begin(), mes_demean.end(), it);
        it = std::copy(mes_env.begin(), mes_env.end(), it);
        std::copy(mes_tkeo_env.begin(), mes_tkeo_env.end(), it);
        emg_data_logger.buffer(mes_log_row);
        
        if (Keyboard::is_key_pressed(Key::Enter)) {
            mes_buffers_full = true;
            for (size_t i = 0; i < emg_channel_count; ++i) {
                if (!emg_electrodes[i].is_buffer_full())
                    mes_buffers_full = false;
            }
            if (mes_buffers_full) {
                if (feature_refract_clock.get_elapsed_time() > feature_refract_time) {
                    for (size_t i = 0; i < emg_channel_count; ++i) {                     
                        emg_electrodes[i].compute_all_features(mes_feature_window_size); // classification features computed here
                        mes_tkeo_env_mean[i] = mean(emg_electrodes[i].get_mes_tkeo_env_buffer_data(mes_buffer_size)); // mean TKEO computed here
                        print("Features computed at time " + stringify(timer.get_elapsed_time_ideal().as_seconds()) + " s are ");
                        print(feat_names);
                        print(emg_electrodes[i].get_all_features());
                        print("MEAN TKEO");
                        print(mes_tkeo_env_mean[i]);
                        print("");
                    }
                    feature_refract_clock.restart();
                }
            }
        }
        
        // check for exit key
        if (Keyboard::is_key_pressed(Key::Escape)) {
            stop = true;
        }

        // kick watchdog
        if (!q8.watchdog.kick())
            stop = true;

        // wait for remainder of sample period
        timer.wait();
    }

    print("Do you want to save the EMG data log? (Y/N)");
    Key key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
    if (key == Key::Y) {
        emg_data_logger.save_data("example_emg_data_log", ".",false);
        emg_data_logger.wait_for_save();
        sleep(seconds(0.5));
    }
    

    disable_realtime();

    return 0;
}


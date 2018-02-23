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
    std::size_t N_emg = q8.analog_input.get_channel_count(); // number of EMG channels
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High)); // set this way for MEII
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High)); // set this way for MEII
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High)); // set this way for MEII

    // make EMG Electrodes
    std::vector<EmgElectrode> emg_electrodes;
    for (size_t i = 0; i < N_emg; ++i) {
        emg_electrodes.emplace_back(q8.analog_input[emg_channel_numbers[i]]);
    }

    // create data logger for MES data
    DataLogger emg_data_logger(WriterType::Buffered, false);
    std::vector<std::string> emg_data_header;
    for (int i = 0; i < N_emg; ++i) {
        emg_data_header.push_back("MES " + stringify(emg_channel_numbers[i]));
    }
    emg_data_logger.set_header(emg_data_header);

    //// create data logger for MES feature window
    //DataLogger emg_feature_logger(WriterType::Buffered, false);
    //std::vector<std::string> emg_feature_header;
    //for (int i = 0; i < N_emg; ++i) {
    //    emg_feature_header.push_back("MES " + stringify(emg_channel_numbers[i]));
    //}
    //emg_feature_logger.set_header(emg_feature_header);
    
    // create local variables
    std::vector<double> mes_raw(N_emg);
    std::vector<double> mes_demean(N_emg);
    std::vector<double> mes_env(N_emg);
    std::vector<double> mes_tkeo_env(N_emg);
    //size_t N_features = emg_electrodes[0].get_all_feautures_count();
    //std::vector<double> mes_all_features_row(N_features);
    //std::vector<std::vector<double>> mes_all_features(N_emg);
    //std::vector<double> mes_window_col(emg_electrodes[0].get_mes_buffer_capacity());
    //std::vector<std::vector<double>> mes_window(N_emg);
    std::vector<double> mes_log_row(N_emg, 0.0);
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
        for (size_t i = 0; i < N_emg; ++i) {
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
        emg_data_logger.buffer(mes_env);
        
        if (Keyboard::is_key_pressed(Key::Enter)) {
            mes_buffers_full = true;
            for (size_t i = 0; i < N_emg; ++i) {
                if (!emg_electrodes[i].is_buffer_full())
                    mes_buffers_full = false;
            }
            if (mes_buffers_full) {
                if (feature_refract_clock.get_elapsed_time() > feature_refract_time) {
                    for (size_t i = 0; i < N_emg; ++i) {
                        emg_electrodes[i].compute_all_features();
                        print(emg_electrodes[i].get_all_features());
                        //mes_window_col = emg_electrodes[i].get_mes_buffer_data();
                        //mes_window[i] = mes_window_col;      
                    }
                    feature_refract_clock.restart();
                    //for (size_t j = 0; j < emg_electrodes[0].get_mes_buffer_capacity(); ++j) {
                    //    for (size_t i = 0; i < N_emg; ++i) {
                    //        mes_log_row[i] = mes_window[i][j];
                    //    }
                    //    //emg_data_logger.buffer(mes_log_row);
                    //    //emg_log.add_row({ mes_log_row[0], mes_log_row[1], mes_log_row[2], mes_log_row[3], mes_log_row[4], mes_log_row[5], mes_log_row[6], mes_log_row[7] });
                    //}
                }
            }
        }
        
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

    //print("Do you want to save the EMG FEATURE LOG? (Y/N)");
    //key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
    //if (key == Key::Y) {
    //    emg_feature_logger.save_data("example_emg_feature_log", ".", false);
    //    emg_feature_logger.wait_for_save();
    //    sleep(seconds(1));
    //}

    disable_realtime();
    return 0;
}


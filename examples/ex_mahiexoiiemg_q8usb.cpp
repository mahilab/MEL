#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp>
#include <MEL/Core/EmgElectrode.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Windows/Keyboard.hpp>
#include <MEL/Logging/Log.hpp>


using namespace mel;

// create global stop variable CTRL-C handler function
ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main(int argc, char *argv[]) {

    // make options
    Options options("ex_mahiexoiiemg_q8usb.exe", "MahiExoIIEmg Q8 USB Demo");
    options.add_options()
        ("c,calibrate", "Calibrates the MAHI Exo-II")
        ("s,scope", "Runs the MAHI Exo-II EMG MelScope demo")
        ("t,trigger","Runs the MAHI Exo-II EMG teleoperation demo in trigger control mode")
        ("d,direct", "Runs the MAHI Exo-II EMG teleoperation demo in direct control mode")
        ("h,help", "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_handler(handler);

    // initialize default MEL logger
    init_logger();

    // make MelShares
    MelShare ms_pos("melscope_pos");
    MelShare ms_vel("melscope_vel");
    MelShare ms_trq("melscope_trq");
    MelShare ms_mes_raw("melscope_mes_raw");
    MelShare ms_mes_demean("melscope_mes_demean");
    MelShare ms_mes_env("melscope_mes_env");
    MelShare ms_mes_tkeo_env("melscope_mes_tkeo_env");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure    
    Q8Usb q8(QOptions(), true, true, { 0, 1, 2, 3, 4, 5, 6, 7 }); // specify all EMG channels
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High));
    if (!q8.identify(7)) {
        LOG(Error) << "Incorrect DAQ";
        return 0;
    }
    std::vector<uint32> emg_channel_numbers = q8.analog_input.get_channel_numbers();
    std::size_t emg_channel_count = q8.analog_input.get_channel_count();

    // create MahiExoII and bind Q8 channels to it
    std::vector<Amplifier> amplifiers;
    std::vector<double> amp_gains;
    for (uint32 i = 0; i < 2; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Low,
                q8.digital_output[i + 1],
                1.8,
                q8.analog_output[i + 1])
        );
    }
    for (uint32 i = 2; i < 5; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Low,
                q8.digital_output[i + 1],
                0.184,
                q8.analog_output[i + 1])
        );
    }
   
    MeiiConfiguration config(q8, q8.watchdog, q8.encoder[{1, 2, 3, 4, 5}], q8.velocity[{1, 2, 3, 4, 5}], amplifiers, q8.analog_input[emg_channel_numbers]);
    MahiExoIIEmg meii(config);
    meii.resize_mes_buffer(200); // set the size of the buffer for calculating EMG-based features

    

    // create data log for EMG data
    DataLogger emg_log(WriterType::Buffered, false);
    std::vector<std::string> log_header;
    log_header.push_back("Time [s]");
    for (std::size_t i = 0; i < emg_channel_count; ++i) {
        log_header.push_back("MES Raw " + stringify(emg_channel_numbers[i]));
        log_header.push_back("MES Demean " + stringify(emg_channel_numbers[i]));
        log_header.push_back("MES Env " + stringify(emg_channel_numbers[i]));
        log_header.push_back("MES TKEO Env " + stringify(emg_channel_numbers[i]));
    }
    emg_log.set_header(log_header);
    std::vector<double> emg_log_row(log_header.size());

    // calibrate - manually zero the encoders (right arm supinated)
    if (result.count("calibrate") > 0) {
        meii.calibrate(stop);
        return 0;
    }

    // display EMG activity on scope
    if (result.count("scope") > 0) {

        LOG(Info) << "Initializing MAHI Exo-II";

        // create initial setpoint
        std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };

        // set up state machine
        uint16 state = 0;
        Time backdrive_time = seconds(3);

        // create data containers
        std::vector<double> rj_positions(meii.N_rj_);
        std::vector<double> rj_velocities(meii.N_rj_);
        std::vector<double> aj_positions(meii.N_aj_);
        std::vector<double> aj_velocities(meii.N_aj_);
        std::vector<double> command_torques(meii.N_aj_);
        std::vector<double> rps_command_torques(meii.N_qs_);
        std::vector<double> mes_raw(emg_channel_count);
        std::vector<double> mes_demean(emg_channel_count);
        std::vector<double> mes_env(emg_channel_count);
        std::vector<double> mes_tkeo_env(emg_channel_count);

        // enable DAQ and exo
        q8.enable();
        meii.enable();

        // initialize controller
        meii.set_rps_control_mode(0);

        // construct timer in hybrid mode to avoid using %100 CPU
        Timer timer(milliseconds(1), Timer::Hybrid);

        // start while loop
        q8.watchdog.start();
        while (!stop) {

            // update all DAQ input channels
            q8.update_input();

            // update MahiExoII kinematics
            meii.update_kinematics();

            // store most recent readings from DAQ
            for (int i = 0; i < meii.N_rj_; ++i) {
                rj_positions[i] = meii[i].get_position();
                rj_velocities[i] = meii[i].get_velocity();
            }
            for (int i = 0; i < meii.N_aj_; ++i) {
                aj_positions[i] = meii.get_anatomical_joint_position(i);
                aj_velocities[i] = meii.get_anatomical_joint_velocity(i);
            }

            switch (state) {
            case 0: // backdrive

                    // command zero torque
                meii.set_joint_torques(command_torques);

                // check for wait period to end
                if (timer.get_elapsed_time() >= backdrive_time) {
                    meii.rps_init_par_ref_.start(meii.get_wrist_parallel_positions(), timer.get_elapsed_time());
                    state = 1;
                }
                break;

            case 1: // initialize rps

                    // calculate commanded torques
                rps_command_torques = meii.set_rps_pos_ctrl_torques(meii.rps_init_par_ref_, timer.get_elapsed_time());
                std::copy(rps_command_torques.begin(), rps_command_torques.end(), command_torques.begin() + 2);

                // check for RPS Initialization target reached
                if (meii.check_rps_init()) {
                    meii.set_rps_control_mode(1); // platform height backdrivable
                    meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                    state = 2;
                    LOG(Info) << "Recording EMG to MelScope and Data Logger";
                }
                break;

            case 2: // read emg

                // emg signal processing
                meii.update_emg();
                mes_raw = meii.get_mes_raw();
                mes_demean = meii.get_mes_demean();
                mes_env = meii.get_mes_env();
                mes_tkeo_env = meii.get_mes_tkeo_env();

                // write to emg data log
                emg_log_row[0] = timer.get_elapsed_time().as_seconds();
                auto it = std::copy(mes_raw.begin(), mes_raw.end(), emg_log_row.begin() + 1);
                it = std::copy(mes_demean.begin(), mes_demean.end(), it);
                it = std::copy(mes_env.begin(), mes_env.end(), it);
                std::copy(mes_tkeo_env.begin(), mes_tkeo_env.end(), it);
                emg_log.buffer(emg_log_row);


                // calculate commanded torques
                command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());

                break;
            }

            // write to MelShares
            ms_pos.write_data(aj_positions);
            ms_vel.write_data(aj_velocities);
            ms_trq.write_data(command_torques);
            ms_mes_raw.write_data(mes_raw);
            ms_mes_demean.write_data(mes_demean);
            ms_mes_env.write_data(mes_env);
            ms_mes_tkeo_env.write_data(mes_tkeo_env);

            // update all DAQ output channels
            q8.update_output();

            // kick watchdog
            if (!q8.watchdog.kick() || meii.any_limit_exceeded())
                stop = true;

            // wait for remainder of sample period
            timer.wait();

        } // end while loop

        print("Do you want to save the EMG data log? (Y/N)");
        Key key = Keyboard::wait_for_any_keys({ Key::Y, Key::N });
        if (key == Key::Y) {
            emg_log.save_data("example_meii_emg_data_log", ".", false);
            emg_log.wait_for_save();
            emg_log.clear_data();
            //sleep(seconds(0.5));
        }

    } // display EMG activity on scope


    // teleoperate the MAHI Exo-II in EMG triggered
    if (result.count("trigger") > 0) {
    } // teleoperate the MAHI Exo-II in EMG triggered

    disable_realtime();
    return 0;
}


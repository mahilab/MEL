#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Math/TeagerKaiserEnergyOperator.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Logging/DataLog.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Math/Filter.hpp>
#include <MEL/Math/Rectifier.hpp>
#include <MEL/Math/SignalProcessor.hpp>

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

    // make MelShares
    MelShare ms_pos("melscope_pos");
    MelShare ms_vel("melscope_vel");
    MelShare ms_trq("melscope_trq");
    MelShare ms_emg("melscope_emg");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure
    Q8Usb q8;
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High));
    if (!q8.identify(7)) {
        print("Incorrect DAQ");
        return 0;
    }

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
    MeiiConfiguration config(q8, q8.watchdog, q8.encoder[{1, 2, 3, 4, 5}], q8.velocity[{1, 2, 3, 4, 5}], amplifiers, q8.analog_input[{0, 1, 2, 3, 4, 5, 6, 7}]);
    MahiExoIIEmg meii(config);

    // make EMG Signals
    std::vector<AnalogInput::Channel> emg_channels;
    for (size_t i = 0; i < meii.N_emg_; ++i) {
        emg_channels.push_back(q8.analog_input[i]);
    }

    // make myoelectric signal processors
    std::vector<Filter> hp_filter(meii.N_emg_, Filter({ 0.814254556886246, -3.257018227544984, 4.885527341317476, -3.257018227544984, 0.814254556886246 }, { 1.000000000000000, -3.589733887112175, 4.851275882519415, -2.924052656162457, 0.663010484385890 })); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    std::vector<Rectifier> rect(meii.N_emg_, Rectifier());
    std::vector<Filter> lp_filter(meii.N_emg_, Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 })); // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    std::vector<Filter> tkeo_hp_filter(meii.N_emg_, Filter({ 0.814254556886246, -3.257018227544984, 4.885527341317476, -3.257018227544984, 0.814254556886246 }, { 1.000000000000000, -3.589733887112175, 4.851275882519415, -2.924052656162457, 0.663010484385890 })); // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
    std::vector<TeagerKaiserEnergyOperator> tkeo(meii.N_emg_, TeagerKaiserEnergyOperator());
    std::vector<Rectifier> tkeo_rect(meii.N_emg_, Rectifier());
    std::vector<Filter> tkeo_lp_filter(meii.N_emg_, Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 })); // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
    std::vector<SignalProcessor> mes_standard_processor;
    std::vector<SignalProcessor> mes_tkeo_processor;
    for (int i = 0; i < meii.N_emg_; ++i) {
        mes_standard_processor.push_back(SignalProcessor({ &hp_filter[i], &rect[i], &lp_filter[i] }));
        mes_tkeo_processor.push_back(SignalProcessor({ &tkeo_hp_filter[i], &tkeo[i], &tkeo_rect[i], &tkeo_lp_filter[i] }));
    }

    // create data log for EMG data
    DataLog<double, double, double, double, double> emg_log({ "Time [s]", "Raw EMG Voltage", "Filtered EMG Voltage", "TKEO EMG", "Filtered TKEO EMG" });


    // calibrate - manually zero the encoders (right arm supinated)
    if (result.count("calibrate") > 0) {
        meii.calibrate(stop);
        return 0;
    }

    // display EMG activity on scope
    if (result.count("scope") > 0) {

        // create initial setpoint
        std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };

        // select EMG channel
        size_t emg_channel_select = 2;



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
        std::vector<double> raw_emg(meii.N_emg_);
        std::vector<double> env_emg(meii.N_emg_);
        std::vector<double> tkeo_emg(meii.N_emg_);
        std::vector<double> tkeo_env_emg(meii.N_emg_);
        std::vector<double> emg_share(4);


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
                    print("RPS Mechanism Initialized");
                    meii.set_rps_control_mode(1); // platform height backdrivable
                    meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                    state = 2;
                }
                break;

            case 2: // read emg

                // emg signal processing
                for (int i = 0; i < meii.N_emg_; ++i) {
                    mes_standard_processor[i].update(emg_channels[i].get_value());
                    mes_tkeo_processor[i].update(emg_channels[i].get_value());
                    raw_emg[i] = mes_standard_processor[i].get_unprocessed();
                    env_emg[i] = mes_standard_processor[i].get_processed(2);
                    tkeo_emg[i] = mes_tkeo_processor[i].get_processed(1);
                    tkeo_env_emg[i] = mes_tkeo_processor[i].get_processed(3);
                }

                // store emg signal processing data for sharing
                emg_share[0] = raw_emg[emg_channel_select];
                emg_share[1] = env_emg[emg_channel_select];
                emg_share[2] = tkeo_emg[emg_channel_select];
                emg_share[3] = tkeo_env_emg[emg_channel_select];

                // write to emg data log
                emg_log.add_row({ timer.get_elapsed_time().as_seconds(), raw_emg[emg_channel_select], env_emg[emg_channel_select], tkeo_emg[emg_channel_select], tkeo_env_emg[emg_channel_select] });


                // calculate commanded torques
                command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());

                break;
            }

            // write to MelShares
            ms_pos.write_data(aj_positions);
            ms_vel.write_data(aj_velocities);
            ms_trq.write_data(command_torques);
            ms_emg.write_data(emg_share);

            // update all DAQ output channels
            q8.update_output();

            // kick watchdog
            if (!q8.watchdog.kick() || meii.any_limit_exceeded())
                stop = true;

            // wait for remainder of sample period
            timer.wait();

        } // end while loop

    } // display EMG activity on scope


    // teleoperate the MAHI Exo-II in EMG triggered
    if (result.count("trigger") > 0) {

        // create initial setpoint
        std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };

        // select EMG channel
        size_t emg_channel_select = 0; // between 0 and 7

        // select DoF to be moved
        int dof = 0; // between 0 and 4

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
        std::vector<double> raw_emg(meii.N_emg_);
        std::vector<double> env_emg(meii.N_emg_);
        std::vector<double> tkeo_emg(meii.N_emg_);
        std::vector<double> tkeo_env_emg(meii.N_emg_);
        std::vector<double> emg_share(4);


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
                    print("RPS Mechanism Initialized");
                    meii.set_rps_control_mode(1); // platform height backdrivable
                    meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                    state = 2;
                }
                break;

            case 2: // EMG trigger teleop

                // emg signal processing
                for (int i = 0; i < meii.N_emg_; ++i) {
                    mes_standard_processor[i].update(emg_channels[i].get_value());
                    mes_tkeo_processor[i].update(emg_channels[i].get_value());
                    raw_emg[i] = mes_standard_processor[i].get_unprocessed();
                    env_emg[i] = mes_standard_processor[i].get_processed(2);
                    tkeo_emg[i] = mes_tkeo_processor[i].get_processed(1);
                    tkeo_env_emg[i] = mes_tkeo_processor[i].get_processed(3);
                }

                // store emg signal processing data for sharing
                emg_share[0] = raw_emg[emg_channel_select];
                emg_share[1] = env_emg[emg_channel_select];
                emg_share[2] = tkeo_emg[emg_channel_select];
                emg_share[3] = tkeo_env_emg[emg_channel_select];

                // write to emg data log
                emg_log.add_row({ timer.get_elapsed_time().as_seconds(), raw_emg[emg_channel_select], env_emg[emg_channel_select], tkeo_emg[emg_channel_select], tkeo_env_emg[emg_channel_select] });


                // calculate commanded torques
                command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());

                break;
            }

            // write to MelShares
            ms_pos.write_data(aj_positions);
            ms_vel.write_data(aj_velocities);
            ms_trq.write_data(command_torques);
            ms_emg.write_data(emg_share);

            // update all DAQ output channels
            q8.update_output();


            // kick watchdog
            if (!q8.watchdog.kick() || meii.any_limit_exceeded())
                stop = true;

            // wait for remainder of sample period
            timer.wait();

        } // end while loop

    } // teleoperate the MAHI Exo-II in EMG triggered

    disable_realtime();
    return 0;
}


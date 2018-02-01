#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Devices/VoltPaqX4.hpp>
#include <MEL/Communications/MelNet.hpp>   
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Math/Functions.hpp>
#include <thread>
#include <atomic>

using namespace mel;

// Global data available to all threads
Mutex mutex;
std::vector<double> data = { 0, 0, 0 };
std::atomic<bool> stop = false;

// CTRL+C handler
static void handler(int var) {
    stop = true;
}

// Communications thread function for OpenWrist
void ow_comm_thread_func() {
    print("Starting OpenWrist communications thread");
    MelNet mnet(55001, 55002, "169.254.208.230", false); // nonblocking
    while (!stop) {
        mutex.lock(); // protect data
        std::vector<double> temp = data;
        mutex.unlock();        
        if (mnet.receive_message() == "send_data") {
            mnet.send_data(temp);
        }
        sleep(milliseconds(10)); // ~100 Hz
    }
    print("Terminating OpenWrist communications thread");
}

// Communications thread function for ME-II
void meii_comm_thread_func() {
    print("Starting ME-II communications thread");
    MelNet mnet(55002, 55001, "169.254.67.6", false); // blocking
    while (!stop) {
        std::vector<double> temp;
        while (temp.empty()) {
            mnet.send_message("send_data");
            temp = mnet.receive_data();
        }
        mutex.lock(); // protect data
        data = temp;
        mutex.unlock();
        sleep(milliseconds(10)); // ~100 Hz
    }
    print("Terminating ME-II communications thread");
}

// Main thread
int main(int argc, char *argv[]) {

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    // create options
    Options options("udp_exo_teleop.exe", "OpenWrist + MAHI Exo-II teleoperation demo over MELNet (UDP)");
    options.add_options()
        ("co", "Calibrate OpenWrist")
        ("cm", "Calibrate MAHI Exo-II")
        ("ro", "Run OpenWrist")
        ("rm", "Run MAHI Exo-II")
        ("help", "Prints this help message");
    auto result = options.parse(argc, argv);

    // switch based on request robot
    if (result.count("co") > 0 || result.count("ro") > 0) {
        // configure OpenWrist
        Q8Usb q8;
        QOptions qoptions;
        qoptions.set_update_rate(QOptions::UpdateRate::Fast);
        qoptions.set_analog_output_mode(0, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
        qoptions.set_analog_output_mode(1, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
        qoptions.set_analog_output_mode(2, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
        q8.set_options(qoptions);
        VoltPaqX4 vpx4(q8.digital_output[{ 0, 1, 2 }], q8.analog_output[{ 0, 1, 2 }], q8.digital_input[{0, 1, 2}], q8.analog_input[{ 0, 1, 2 }]);
        OwConfiguration config(
            q8,
            q8.watchdog,
            q8.encoder[{ 0, 1, 2 }],
            q8.velocity[{ 0, 1, 2 }],
            vpx4.amplifiers
        );
        OpenWrist ow(config);
        if (result.count("co") > 0) {
            // calibrate OpenWrist
            ow.calibrate(stop);
            return 0;
        }
        else if (result.count("ro") > 0) {
            // run OpenWrist
            print("Starting OpenWrist main thread");
            // start communications thread
            std::thread comm_thread(ow_comm_thread_func);
            // enable hardware
            q8.enable();
            ow.enable();
            q8.watchdog.start();
            // start control loop
            Timer timer(milliseconds(1), Timer::Hybrid);
            while (!stop) {
                // read and reload DAQs
                q8.update_input();
                q8.watchdog.kick();
                // calculate and set compensation torques
                ow[0].set_torque(ow.compute_gravity_compensation(0) + ow.compute_friction_compensation(0));
                ow[1].set_torque(ow.compute_gravity_compensation(1) + ow.compute_friction_compensation(1));
                ow[2].set_torque(ow.compute_friction_compensation(2) * 0.5);
                // write all DAQs
                q8.update_output();
                // check joint limits
                if (ow.check_all_joint_torque_limits() && ow.check_all_joint_velocity_limits()) {
                    stop = true;
                    break;
                }
                mutex.lock();
                data[0] = ow[0].get_position();
                data[1] = ow[1].get_position();
                data[2] = ow[2].get_position();
                mutex.unlock();
                timer.wait();
            }
            // join comm thread
            comm_thread.join();
            print("Terminating OpenWrist main thread");
            return 0;
        }
    }
    else if (result.count("cm") > 0 || result.count("rm") > 0) {
        // make Q8 USB and configure
        Q8Usb q8;
        q8.digital_output.set_enable_values(std::vector<logic>(8, HIGH));
        q8.digital_output.set_disable_values(std::vector<logic>(8, HIGH));
        q8.digital_output.set_expire_values(std::vector<logic>(8, HIGH));
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
                    Amplifier::TtlLevel::Low,
                    q8.digital_output[i + 1],
                    1.8,
                    q8.analog_output[i + 1])
            );
        }
        for (uint32 i = 2; i < 5; ++i) {
            amplifiers.push_back(
                Amplifier("meii_amp_" + std::to_string(i),
                    Amplifier::TtlLevel::Low,
                    q8.digital_output[i + 1],
                    0.184,
                    q8.analog_output[i + 1])
            );
        }
        MeiiConfiguration config(q8, q8.watchdog, q8.encoder[{1, 2, 3, 4, 5}], q8.velocity[{1, 2, 3, 4, 5}], amplifiers);
        MahiExoII meii(config);

        

        if (result.count("cm") > 0) {
            meii.calibrate(stop);
            return 0;
        }
        else if (result.count("rm") > 0) {
            // run ME-II
            print("Starting ME-II main thread");

            // create initial setpoint and ranges
            std::vector<double> setpoint = { -35 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD, 0 * DEG2RAD,  0.10 };
            std::vector<std::vector<double>> setpoint_ranges = { { -90 * DEG2RAD, 0 * DEG2RAD },
            { -90 * DEG2RAD, 90 * DEG2RAD },
            { -15 * DEG2RAD, 15 * DEG2RAD },
            { -15 * DEG2RAD, 15 * DEG2RAD },
            { 0.08, 0.115 } };

            // set up state machine
            uint16 state = 0;
            Time backdrive_time = seconds(3);

            // create data containers
            std::vector<double> command_torques(meii.N_aj_);
            std::vector<double> rps_command_torques(meii.N_qs_);

            // enable DAQ and exo
            q8.enable();
            meii.enable();

            // initialize controller
            meii.set_rps_control_mode(0);

            // start communications thread
            std::thread comm_thread(meii_comm_thread_func);
            // start control loop
            Timer timer(milliseconds(1), Timer::Hybrid);
            q8.watchdog.start();
            while (!stop) {

                // update all DAQ input channels
                q8.update_input();

                // update MahiExoII kinematics
                meii.update_kinematics();

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
                        meii.set_rps_control_mode(2); // platform height non-backdrivable
                        meii.anat_ref_.start(setpoint, meii.get_anatomical_joint_positions(), timer.get_elapsed_time());
                        state = 2;
                    }
                    break;

                case 2: // setpoint control

                    mutex.lock();
                    setpoint[1] = data[0];
                    setpoint[2] = data[1];
                    setpoint[3] = data[2];
                    mutex.unlock();          

                    // update and saturate setpoint
                    for (int i = 0; i < meii.N_aj_; ++i) {
                        setpoint[i] = saturate(setpoint[i], setpoint_ranges[i][0], setpoint_ranges[i][1]);
                    }
                    meii.anat_ref_.set_ref(setpoint, timer.get_elapsed_time());

                    // calculate commanded torques
                    command_torques = meii.set_anat_pos_ctrl_torques(meii.anat_ref_, timer.get_elapsed_time());


                    break;
                }

                // update all DAQ output channels
                q8.update_output();

                // kick watchdog
                if (!q8.watchdog.kick() || meii.check_all_joint_limits())
                    stop = true;

            }
            // join comm thread
            comm_thread.join();
            print("Terminating ME-II main thread");
            return 0;
        }
    }
    else {
        print(options.help());
    }
    return 0;
}



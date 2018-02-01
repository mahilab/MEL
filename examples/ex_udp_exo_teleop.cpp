#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Devices/VoltPaqX4.hpp>
#include <MEL/Communications/MelNet.hpp>   
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Mutex.hpp>
#include <MEL/Utility/Options.hpp>
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
        if (mnet.receive_message() == "send_data")
            mnet.send_data(temp);
        sleep(milliseconds(10)); // ~100 Hz
    }
    print("Terminating OpenWrist communications thread");
}

// Communications thread function for ME-II
void meii_comm_thread_func() {
    print("Starting ME-II communications thread");
    MelNet mnet(55002, 55001, "169.254.67.6", true); // blocking
    while (!stop) {
        mnet.send_message("send_data");
        std::vector<double> temp = mnet.receive_data();
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
                // lock mutex
                Lock lock(mutex);
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
                data[0] = ow[0].get_position();
                data[1] = ow[1].get_position();
                data[2] = ow[2].get_position();
                timer.wait();
            }
            // join comm thread
            comm_thread.join();
            print("Terminating OpenWrist main thread");
            return 0;
        }
    }
    else if (result.count("cm") > 0 || result.count("rm") > 0) {
        // configure MEI-II
        Q8Usb q8;
        // ...
        if (result.count("cm") > 0) {
            // calibrate ME-II
            // ...
            return 0;
        }
        else if (result.count("rm") > 0) {
            // run ME-II
            print("Starting ME-II main thread");
            // start communications thread
            std::thread comm_thread(meii_comm_thread_func);
            // start control loop
            Timer timer(milliseconds(1), Timer::Hybrid);
            while (!stop) {
                Lock lock(mutex);
                print(data);
                timer.wait();
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



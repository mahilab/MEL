#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Utility/System.hpp>
#include <vector>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Functions.hpp>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main(int argc, char *argv[]) {

    // make options
    Options options("openwrist_q8usb.exe", "OpenWrist Q8 USB Demo");
    options.add_options()
        ("c,calibrate",    "Calibrates the OpenWrist")
        ("t,transparency", "Puts the OpenWrist in transparency mode")
        ("s,setpoint",     "Runs OpenWrist MelScope set-point demo")
        ("h,help",         "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB that's configured for current control with VoltPAQ-X4
    QOptions qoptions;
    qoptions.set_update_rate(QOptions::UpdateRate::Fast);
    qoptions.set_analog_output_mode(0, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    qoptions.set_analog_output_mode(1, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    qoptions.set_analog_output_mode(2, QOptions::AoMode::CurrentMode1, 0, 2.0, 20.0, 0, -1, 0, 1000);
    Q8Usb q8(qoptions);

    // create OpenWrist and bind Q8 channels to it
    OwConfiguration config(
        q8,
        q8.watchdog,
        std::vector<Amplifier::TtlLevel>(3, Amplifier::TtlLevel::High),
        q8.digital_output[{ 0, 1, 2 }],
        { 1.0, 1.0, 1.0 },
        q8.analog_output[{ 0, 1, 2 }],
        {Limiter(), Limiter(), Limiter()},
        std::vector<Amplifier::TtlLevel>(3, Amplifier::TtlLevel::High),
        q8.digital_input[{0, 1, 2}],
        {1.0, 1.0, 1.0},
        q8.analog_input[{ 0, 1, 2 }],
        q8.encoder[{ 0, 1, 2 }],
        q8.velocity[{ 0, 1, 2 }]
    );
    OpenWrist ow(config);

    // run calibration script
    if (result.count("calibrate") > 0) {
        ow.calibrate(stop);
        return 0;
    }

    // enter transparency mode
    if (result.count("transparency") > 0) {
        ow.transparency_mode(stop);
        return 0;
    }

    // setpoint control with MelScope
    if (result.count("setpoint") > 0) {
        MelShare ms("ow_setpoint");
        std::vector<double> positions(3, 0.0);
        std::vector<double> torques(3, 0.0);
        ms.write_data(positions);
        q8.enable();
        ow.enable();
        q8.watchdog.start();
        Timer timer(milliseconds(1), Timer::Hybrid);
        while (!stop) {
            q8.update_input();
            positions = ms.read_data();
            positions[0] = saturate(positions[0], 80);
            positions[1] = saturate(positions[1], 60);
            positions[2] = saturate(positions[2], 30);
            torques[0] = ow.pd_controllers_[0].move_to_hold(positions[0] * DEG2RAD, ow[0].get_position(), 30 * DEG2RAD, ow[0].get_velocity(), 0.001, DEG2RAD, 10 * DEG2RAD);
            torques[1] = ow.pd_controllers_[1].move_to_hold(positions[1] * DEG2RAD, ow[1].get_position(), 30 * DEG2RAD, ow[1].get_velocity(), 0.001, DEG2RAD, 10 * DEG2RAD);
            torques[2] = ow.pd_controllers_[2].move_to_hold(positions[2] * DEG2RAD, ow[2].get_position(), 30 * DEG2RAD, ow[2].get_velocity(), 0.001, DEG2RAD, 10 * DEG2RAD);
            ow.set_joint_torques(torques);
            if (!q8.watchdog.kick() || ow.check_all_joint_limits())
                stop = true;
            q8.update_output();
            timer.wait();
        }
    }

    disable_realtime();
    return 0;
}


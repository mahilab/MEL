#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
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
    Options options("mahiexoii_q8usb.exe", "MahiExoII Q8 USB Demo");
    options.add_options()
        ("t,test", "...")
        ("h,help", "Prints this help message");

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
    Q8Usb q8;

    // create MahiExoII and bind Q8 channels to it
    std::vector<Amplifier> amplifiers;
    std::vector<double> amp_gains;
    for (std::size_t i = 0; i < 2; ++i) {
        amplifiers.push_back(
            Amplifier("meii_amp_" + std::to_string(i),
                Amplifier::TtlLevel::Low,
                q8.digital_output[i + 1],
                1.8,
                q8.analog_output[i + 1])
        );
    }
    for (std::size_t i = 2; i < 5; ++i) {
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

    // setpoint control with MelScope
    if (result.count("test") > 0) {
        q8.enable();
        meii.enable();
        q8.watchdog.start();
        Timer timer(milliseconds(1));
        while (!stop) {
            //q8.update_input();
            //q8.update_output();
            timer.wait();
        }
    }

    disable_realtime();
    return 0;
}


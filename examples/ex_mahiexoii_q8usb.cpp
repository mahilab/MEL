#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Utility/System.hpp>
#include <vector>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/DataLog.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/RingBuffer.hpp>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main(int argc, char *argv[]) {

    // make options
    Options options("mahiexoii_q8usb.exe", "MahiExoII Q8 USB Demo");
    options.add_options()
        ("c,calibrate", "Calibrates the MAHI Exo-II")
        ("t,test", "...")
        ("h,help", "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_c_handler(handler);

    // make MelShares
    MelShare ms("melscope1");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB
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

    // calibrate - manually zero the encoders
    if (result.count("calibrate") > 0) {
        meii.calibrate(stop);
        return 0;
    }

    // setpoint control with MelScope
    if (result.count("test") > 0) {
        std::vector<double> positions(1);
        q8.enable();
        q8.digital_output.set_disable_values(std::vector<logic>(8, HIGH));
        q8.digital_output.set_expire_values(std::vector<logic>(8, HIGH));
        q8.watchdog.start();
        meii.enable();
        Clock clock;
        Timer timer(milliseconds(1), Timer::Sleep);
        RingBuffer<int64> e_time(500);
        while (!stop) {
            
            clock.restart();
            q8.update_input();
            positions[0] = meii[0].get_position();
            ms.write_data(positions);
            if (!q8.watchdog.kick())
                stop = true;
            timer.wait();
            e_time.push_back(clock.get_elapsed_time().as_microseconds());
            
        }
        for (int i = 0; i < e_time.size(); ++i) {
            print(e_time[i]);
        }
        
    }

    disable_realtime();
    return 0;
}


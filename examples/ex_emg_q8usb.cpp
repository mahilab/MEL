#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Core/EmgSignal.hpp>
#include <MEL/Utility/Timer.hpp>
#include <vector>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

// number of EMG channels
static const int N_emg = 8;

int main(int argc, char *argv[]) {

    // make options
    Options options("ex_emg_q8usb.exe", "EMG Q8 USB Demo");
    options.add_options()
        ("s,scope", "Scope EMG")
        ("h,help", "Prints this help message");

    auto result = options.parse(argc, argv);

    if (result.count("help") > 0) {
        print(options.help());
        return 0;
    }

    // register ctrl-c handler
    register_ctrl_handler(handler);

    // make MelShares
    MelShare ms_emg("melscope_emg");

    // enable Windows realtime
    enable_realtime();

    // make Q8 USB and configure
    Q8Usb q8;
    q8.digital_output.set_enable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_disable_values(std::vector<Logic>(8, High));
    q8.digital_output.set_expire_values(std::vector<Logic>(8, High));

    // make EMG Signals
    std::vector<EmgSignal> emg_signals;
    const int emg_buffer_size = 1000;
    for (int i = 0; i < N_emg; ++i) {
        emg_signals.push_back(EmgSignal(q8.analog_input[i]));
    }

    // read live EMG
    if (result.count("scope") > 0) {

        // create data containers
        std::vector<double> raw_emg(N_emg);

        // enable DAQ
        q8.enable();

        // construct timer in hybrid mode to avoid using %100 CPU
        Timer timer(milliseconds(1), Timer::Hybrid);

        // start loop
        q8.watchdog.start();
        while (!stop) {

            // update all DAQ input channels
            q8.update_input();

            // store most recent readings from DAQ
            for (int i = 0; i < N_emg; ++i) {
                raw_emg[i] = emg_signals[i].get_voltage();
            }

            // write to MelShares
            ms_emg.write_data(raw_emg);

            // kick watchdog
            if (!q8.watchdog.kick())
                stop = true;

            // wait for remainder of sample period
            timer.wait();

        }


    }

    disable_realtime();
    return 0;
}


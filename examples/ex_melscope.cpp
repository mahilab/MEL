#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Communications/MelNet.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Options.hpp>

// To see this example in action, start this exectable then run MelScope.pyw
// and open the provided example.scope file.

using namespace mel;

ctrl_bool stop = false;
int handler(unsigned long param) {
    stop = true;
    return 1;
}

int main(int argc, char *argv[]) {

    Options options("melscope.exe", "MEL Scope Demo");
    options.add_options()
        ("r", "Remote Address", value<std::string>());
    auto result = options.parse(argc, argv);
    std::string remote_address;
    if (result.count("r") > 0)
        remote_address = result["r"].as<std::string>();

    print(remote_address);

    // register CTRL-C handler
    register_ctrl_handler(handler);

    // make MelShares
    MelShare ms1("melscope1");

    // make MelNets
    MelNet mn1(55001, 55002, IpAddress(remote_address), false);

    // create data buffers so we don't have to make them in each loop iteration
    std::vector<double> data1(2);
    std::vector<double> data2 = {1.0, 2.0};
    std::vector<double> data3(2);

    // write intial values for the amplitudes
    // ms2.write_data(data2);

    // create waveforms
    Waveform sin_wave(Waveform::Sin,    seconds(2));
    Waveform sqr_wave(Waveform::Square, seconds(2));
    Waveform tri_wave(Waveform::Triangle, seconds(2));
    Waveform saw_wave(Waveform::Sawtooth, seconds(2));

    print("Running MEL Scope Example ... ");

    // create a Clock (note a Timer could be used too)
    Clock clock;

    // data loop
    while (!stop) {
        // read in amplitudes
        // data2 = ms2.read_data();
        // set amplitudes
        sin_wave.amplitude_ = data2[0];
        sqr_wave.amplitude_ = data2[1];
        // evaluate waveforms
        data1[0] = sin_wave.evaluate(clock.get_elapsed_time());
        data1[1] = sqr_wave.evaluate(clock.get_elapsed_time());
        data3[0] = tri_wave.evaluate(clock.get_elapsed_time());
        data3[1] = saw_wave.evaluate(clock.get_elapsed_time());
        // write waveform data
        ms1.write_data(data1);
        // update MelNet
        if (mn1.check_request())
            mn1.send_data(data3);
        // let the thread sleep a little so we don't use 100% CPU
        sleep(milliseconds(1));
    }

    print("Terminated");

    return 0;
}

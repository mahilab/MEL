#include <MEL/Communications/MelNet.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Options.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Logging/Log.hpp>
#ifdef _WIN32
#include <MEL/Communications/Windows/MelShare.hpp>
#endif

// To see this example in action, start this exectable then run MelScope.pyw
// and open the provided example.scope file.

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    stop = true;
    return true;
}

int main(int argc, char* argv[]) {
    Options options("melscope.exe", "MEL Scope Demo");
    options.add_options()("r", "Remote Address", value<std::string>());
    auto result = options.parse(argc, argv);
    std::string remote_address;
    if (result.count("r") > 0)
        remote_address = result["r"].as<std::string>();

    // register CTRL-C handler
    register_ctrl_handler(handler);

    init_logger();

    // make MelNet
    MelNet mn(55001, 55002, IpAddress(remote_address), false);

#ifdef _WIN32
    // make MelShare (Windows only, and prefered over MelNet)
    MelShare ms("melscope1");
#endif

    // create data buffers so we don't have to make them in each loop iteration
    std::vector<double> data_ms(2);
    std::vector<double> data_mn(2);

    // create waveforms
    Waveform sin_wave(Waveform::Sin, seconds(2));
    Waveform sqr_wave(Waveform::Square, seconds(2));
    Waveform tri_wave(Waveform::Triangle, seconds(2));
    Waveform saw_wave(Waveform::Sawtooth, seconds(2));

    print("Running MEL Scope Example ... ");

    Timer timer(milliseconds(1), Timer::Hybrid);
    while (!stop) {
        // evaluate waveforms
        data_ms[0] = sin_wave.evaluate(timer.get_elapsed_time());
        data_ms[1] = sqr_wave.evaluate(timer.get_elapsed_time());

        data_mn[0] = tri_wave.evaluate(timer.get_elapsed_time());
        data_mn[1] = saw_wave.evaluate(timer.get_elapsed_time());

#ifdef _WIN32
        // write waveform data
        ms.write_data(data_ms);
#endif

        // update MelNet
        if (mn.check_request())
            mn.send_data(data_mn);

        timer.wait();
    }

    print("Terminated");

    return 0;
}

#include <MEL/Communications/MelShare.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>

// To see this example in action, start this exectable then run MelScope.pyw
// and open the provided example.scope file.

using namespace mel;

// make a CTRL-C handler and global stop variable
static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    // register CTRL-C handler
    register_ctrl_c_handler(handler);

    // make MelShares
    MelShare ms1("melscope1");
    MelShare ms2("melscope2");
    MelShare ms3("melscope3");

    // create data buffers so we don't have to make them in each loop iteration
    std::vector<double> data1(2);
    std::vector<double> data2 = {1.0, 2.0};
    std::vector<double> data3(2);

    // write intial values for the amplitudes
    ms2.write_data(data2);

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
        data2 = ms2.read_data();
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
        ms3.write_data(data3);
        // let the thread sleep a little so we don't use 100% CPU
        sleep(milliseconds(1));
    }

    print("Terminated");

    return 0;
}

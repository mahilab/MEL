#include <MEL/Communications/MelShare.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Utility/Console.hpp>

using namespace mel;

static bool stop = false;
static void handler(int var) {
    stop = true;
}

int main() {

    register_ctrl_c_handler(handler);

    print("Generating data ... ");

    MelShare ms1("melscope1");
    MelShare ms2("melscope2");

    std::vector<double> data1(2);
    std::vector<double> data2 = {1.0, 2.0};
    ms2.write_data(data2);

    Waveform sin_wave(Waveform::Sin, seconds(1));
    Waveform cos_wave(Waveform::Cos, seconds(1));

    Timer timer(milliseconds(10), Timer::Hybrid);

    while (!stop) {

        data2 = ms2.read_data();
        sin_wave.amplitude_ = data2[0];
        cos_wave.amplitude_ = data2[1];

        data1[0] = sin_wave.evaluate(timer.get_elapsed_time());
        data1[1] = cos_wave.evaluate(timer.get_elapsed_time());
        ms1.write_data(data1);

        timer.wait();
    }

    print("Finished");
}

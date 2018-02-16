#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/RingBuffer.hpp>
#include <MEL/Exoskeletons/OpenWrist/OpenWrist.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Communications/Windows/MelShare.hpp>

using namespace mel;

int main(int argc, char const* argv[]) {

    Waveform sinwave(Waveform::Sin, seconds(1), 5.0, 1.0);

    Timer timer(milliseconds(1), Timer::Hybrid);

    std::vector<double> data(1);

    MelShare ms("melshare");

    while(true) {

        data[0] = sinwave.evaluate(timer.get_elapsed_time());

        ms.write_data(data);

        timer.wait();
    }

}

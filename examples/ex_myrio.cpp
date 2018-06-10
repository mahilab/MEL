#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <string>

using namespace mel;

int main(int argc, char **argv)
{
    init_logger(Verbose,Verbose);

    MyRio myrio("myrio");
    myrio.enable();

    Waveform sinwave(Waveform::Sin, seconds(1), 10);

    Timer timer(hertz(1000));
    while (timer.get_elapsed_time() < seconds(1)) {
        myrio.analog_input_C.update();
        print(myrio.analog_input_C[1].get_value());
        timer.wait();
    }

    print(timer.get_elapsed_time_actual());

}


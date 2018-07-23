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
    Waveform sinwave(Waveform::Sin, seconds(1), 2.5, 2.5);
    Timer timer(hertz(100));
    while (timer.get_elapsed_time() < seconds(1)) {
        myrio.update_input();
        print(myrio.A.AI[0].get_value());
        double value = sinwave.evaluate(timer.get_elapsed_time());
        myrio.A.AO[0].set_value(value);
        myrio.update_output();
        timer.wait();
    }
}


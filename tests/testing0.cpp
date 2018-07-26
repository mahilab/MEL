#include <MEL/Core/Timer.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Console.hpp>
#include <string>

using namespace mel;
using namespace std;

int main(int argc, char** argv) {
    init_logger(Verbose, Verbose);
    MyRio myrio("myrio");
    myrio.enable();
    Timer timer(hertz(100));

    myrio.C.DIO[0].set_direction(Out);

    while (timer.get_elapsed_time() < seconds(60)) {
        myrio.update_input();

        if (myrio.C.DIO[1].get_value() == High)
            print(timer.get_elapsed_ticks());
        if (myrio.C.DIO[2].get_value() == High)
            myrio.C.DIO[0].set_value(High);
        else
            myrio.C.DIO[0].set_value(Low);



        myrio.update_output();
        timer.wait();
    }
}

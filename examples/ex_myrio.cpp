#include <MEL/Core/Timer.hpp>
#include <MEL/Daq/NI/MyRio/MyRio.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Communications/MelNet.hpp>
#include <string>

using namespace mel;
using namespace std;

int main(int argc, char** argv) {
    MelNet mn(55001, 55002, "10.0.0.117", false);


    MyRio myrio("myrio");
    myrio.enable();
    Timer timer(hertz(1000), Timer::Hybrid);
    Waveform sinwave(Waveform::Sin, seconds(1), 5);

    std::vector<double> AB(2);


    while (timer.get_elapsed_time() < seconds(60)) {
        myrio.update_input();

        AB[0] = myrio.C.DIO[0].get_value();
        AB[1] = myrio.C.DIO[2].get_value();

        // update MelNet
        print(AB);

        myrio.update_output();
        timer.wait();
    }
}

#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <bitset>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Daq/Quanser/Q8Usb.hpp>
#include <MEL/Math/Butterworth.hpp>

using namespace mel;

ctrl_bool g_stop_flag(false);

bool my_handler(CtrlEvent event)
{
    if (event == CtrlEvent::CtrlC)
        g_stop_flag = true;
    return true;
}

int main(int argc, char const *argv[])
{
    register_ctrl_handler(my_handler);

    S826 s826;
    s826.open();
    s826.enable();
    // s826.AI.set_settling_time(microseconds(3));
    s826.encoder[0].zero();

    Timer timer(hertz(1000));
    timer.disable_warnings();
    Time t;

    Waveform sinewave(Waveform::Sin, seconds(1));

    //  s826.encoder[0].reset_count(0);
     s826.encoder[0].set_units_per_count(360.0 / 512);

     MelShare ms("s826");
     std::vector<double> buffer(5);

     Differentiator diff1;
     Butterworth filt(2, hertz(100), hertz(1000));
    //  Differentiator diff2;

    while (!g_stop_flag)
    {
        s826.AI.update();
        s826.encoder.update();
        buffer[0] = sinewave(t);
        buffer[1] = s826.AI[0].get_value();
        buffer[2] = s826.encoder[0].get_position();
        buffer[3] = s826.encoder[0].get_velocity();
        double vel = filt.update(buffer[3],t); 
        buffer[4] = vel;
        s826.AO[0].set_value(buffer[0]);
        s826.AO.update();
        ms.write_data(buffer);
        t = timer.wait();
    }

    print("Wait Ratio:", timer.get_wait_ratio());
    print("Misses:", timer.get_misses());

    return 0;
}

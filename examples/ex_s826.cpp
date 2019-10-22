#include <MEL/Daq/Sensoray/S826.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Core/Timer.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Communications/MelShare.hpp>
#include <bitset>
#include <MEL/Math/Differentiator.hpp>

using namespace mel;

ctrl_bool g_stop_flag(false);

bool my_handler(CtrlEvent event) {
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

     s826.AO[0].set_value(10);
     s826.AO[0].update();
     
     Timer timer(hertz(1000));
     Time t1;
     Time t2;

     Waveform sinewave(Waveform::Sin, seconds(1));

     s826.encoder[0].reset_count(0);
     s826.encoder[0].set_units_per_count(360.0 / 2048);

     MelShare ms("s826");
     std::vector<double> buffer(3);

     Differentiator diff1;
     Differentiator diff2;

     while (!g_stop_flag) {
         s826.encoder[0].update();
         t2 += s826.encoder.timestamps_[0];
         auto pos = s826.encoder[0].get_position();
         auto vel1 = diff1.update(pos, t1);
         auto vel2 = diff2.update(pos, t2);
         ms.write_data({ pos, vel1, vel2 });
         t1 = timer.wait();
     }

     print(timer.get_misses());


    return 0;
}

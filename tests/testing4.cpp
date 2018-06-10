#include <MEL/Core/Timer.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Logging/Log.hpp>
#include <cstdlib>
#include <array>

using namespace mel;
using namespace std;

std::array<double,3> q;

double compute_gravity_compensation(uint32 joint) {
    q = { 0.01*(rand() % 100), .01*(rand() % 100), .01*(rand() % 100) };
    if (joint == 0)
        return 1.3939*sin(q[0]) - 0.395038*cos(q[0]) + 0.351716*cos(q[0])*cos(q[1]) + 0.0826463*cos(q[0])*sin(q[1]) + 0.0397738*cos(q[2])*sin(q[0]) - 0.0929844*sin(q[0])*sin(q[2]) - 0.0929844*cos(q[0])*cos(q[2])*sin(q[1]) - 0.0397738*cos(q[0])*sin(q[1])*sin(q[2]);
    else if (joint == 1)
        return -8.50591e-18*sin(q[0])*(4.13496e16*sin(q[1]) - 9.71634e15*cos(q[1]) + 1.09317e16*cos(q[1])*cos(q[2]) + 4.67601e15*cos(q[1])*sin(q[2]));
    else if (joint == 2)
        return 0.0929844*cos(q[0])*cos(q[2]) + 0.0397738*cos(q[0])*sin(q[2]) + 0.0929844*sin(q[0])*sin(q[1])*sin(q[2]) - 0.0397738*cos(q[2])*sin(q[0])*sin(q[1]);
    else
        return 0.0;
}

int main(int argc, char* argv[]) {

    init_logger();

    volatile double x,y,z;

    Timer clock(hertz(1000));
    for (int i = 0; i < 5000; ++i) {
        x = compute_gravity_compensation(0);
        y = compute_gravity_compensation(1);
        z = compute_gravity_compensation(1);
        clock.wait();
    }
    print(clock.get_elapsed_time_actual());
    print(x+y+z);

    return 0;
}

#include <MEL/Communications/Windows/MelShare.hpp>
#include <MEL/Math/Constants.hpp>
#include <MEL/Math/Differentiator.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Math/Integrator.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/Timer.hpp>

using namespace mel;

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    print("Ctrl+C Pressed");
    stop = true;
    return true;
}

int main(int argc, char const* argv[]) {
    register_ctrl_handler(handler);

    std::vector<double> data(5);
    MelShare ms("ms_math");

    Timer timer(milliseconds(1), Timer::Hybrid);

    Differentiator differentiator =
        Differentiator(Differentiator::Technique::CentralDifference);
    Integrator integrator = Integrator(5, Integrator::Technique::Trapezoidal);

    while (!stop) {
        double t = timer.get_elapsed_time_ideal().as_seconds();

        // x = 1/2 * [ cos(t) + 7 * cos(7t) ]
        data[0] = 0.5 * (mel::cos(t) + 7.0 * mel::cos(7.0 * t));
        // dx/dt = 1/2 * [-sin(t) - 49 * sin(7t)]
        data[1] = 0.5 * (-mel::sin(t) - 49.0 * mel::sin(7.0 * t));
        // dx/dt ~ differentiate(x);
        data[2] = differentiator.differentiate(data[0], timer.get_elapsed_time_ideal());
        // S(x) = sin(4t) * cos(3t) + 5
        data[3] = mel::sin(4 * t) * mel::cos(3 * t) + 5;
        // S(x) ~ integrate(dx/dt)
        data[4] = integrator.integrate(data[0], timer.get_elapsed_time_ideal());

        ms.write_data(data);
        timer.wait();
    }

    return 0;
}

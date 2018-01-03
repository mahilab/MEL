#include <MEL/Communications/MelShare.hpp>
#include <MEL/Math/Integrator.hpp>
#include <MEL/Math/Waveform.hpp>
#include <MEL/Utility/Timer.hpp>
#include <MEL/Utility/System.hpp>
#include <array>

using namespace mel;

int main() {

    enable_realtime();

    // **Open example.scope in MELScope to see this example in action**

    // Create our MELShare maps. We will make one map, "integrals", to demonstate
    // MEL's Integrator class. We will also make three other maps, "cpp2py",
    // "py2cpp_ampl", and "py2cpp_freq" to deomstate bidirectional communication in
    // MELShare. While bidirectional communcation over a single MELShare map is possible
    // with the read_write() functions, it can be tricky to get the timing right.
    // It's best to just keep two maps open for this purpose.
    MelShare integrals("integrals");
    MelShare cpp2py("cpp2py");
    MelShare py2cpp_ampl("py2cpp_ampl");
    MelShare py2cpp_freq("py2cpp_freq");

    // create the data buffers for each map
    std::array<double, 4> integrals_data = { 0, 0, 0, 0 };
    std::array<double, 4> cpp2py_data = { 0, 0, 0, 0 };
    std::array<double, 4> py2cpp_ampl_data = { 10, 8, 6, 4 };
    std::array<double, 4> py2cpp_freq_data = { 0.4, 0.6, 0.8, 1.0 };

    // create some cyclic Waveforms
    Waveform sin_wave(Waveform::Sin,      seconds(4), 10);
    Waveform sqr_wave(Waveform::Square,   seconds(3), 8);
    Waveform tri_wave(Waveform::Triangle, seconds(2), 6);
    Waveform saw_wave(Waveform::Sawtooth, seconds(1), 4);

    // create a MEL Integrator for doing some integration. note the initial value 5
    Integrator integrator(5, Integrator::Technique::Trapezoidal);

    // In this example, we will have the data in cpp2py be a funcion of the data
    // from py2cpp. Specifically, we will generate some periodic data, and use
    // scalers from py2cpp to change the amplitudes and frequencies. For that
    // reason, we will start off by actually writing py2cpp so that it has some
    // intial values.
    py2cpp_ampl.write(py2cpp_ampl_data);
    py2cpp_freq.write(py2cpp_freq_data);

    // create a Timer to control the rate of our data generating loop
    Timer timer(milliseconds(1)); // 1000 Hz clock

    // run the data loop for 60 seconds
    while (timer.get_elapsed_time() < seconds(60)) {

        Time t = timer.get_elapsed_time();

        // t
        integrals_data[0] = t.as_seconds();
        // dx/dt = 1/2 * [ cos(t) + 7 * cos(7t) ]
        integrals_data[1] = 0.5 * (cos(t.as_seconds()) + 7 * cos(7 * t.as_seconds()));
        // x = sin(4t) * cos(3t) + 5
        integrals_data[2] = sin(4 * t.as_seconds()) * cos(3 * t.as_seconds()) + 5;
        // x ~ integrate(dx/dt)
        integrals_data[3] = integrator.integrate(integrals_data[1], t);

        // write integrals
        integrals.write(integrals_data);

        // read data from py2cpp_ampl and py2cpp_freq
        py2cpp_ampl.read(py2cpp_ampl_data);
        py2cpp_freq.read(py2cpp_freq_data);

        // generate new data for cpp2py
        cpp2py_data[0] = sin_wave.evaluate(t);
        cpp2py_data[1] = sqr_wave.evaluate(t);
        cpp2py_data[2] = tri_wave.evaluate(t);
        cpp2py_data[3] = saw_wave.evaluate(t);

        // write cpp2py
        cpp2py.write(cpp2py_data);

        // wait the timer
        timer.wait();
    }

    disable_realtime();

    return 0;
}

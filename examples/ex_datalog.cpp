#include <MEL/Core/Timer.hpp>
#include <MEL/Logging/DataLogger.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Math/Functions.hpp>
#include <MEL/Utility/Console.hpp>
#include <MEL/Utility/System.hpp>

using namespace mel;

// Usage:
// Run the example to generate two data logs that are written in real time. One
// writes to a file immediately, while the next writes to a buffer and then
// writes all data to a file at the end. Change the duration of writing to each
// of these buffers by changing the variables immediate_write_duration and
// buffered_write_duration in this script.

ctrl_bool stop(false);
bool handler(CtrlEvent event) {
    if (event == CtrlEvent::CtrlC)
        stop = true;
    return true;
}

int main() {
    // register ctrl-c handler
    register_ctrl_handler(handler);

    enable_realtime();

    // duration of testing
    Time immediate_write_duration = seconds(1);
    Time buffered_write_duration  = seconds(10);

    // chaotic gauss map
    double alpha = 4.90;
    double beta  = -0.58;
    double x     = 0.1123565612351;

    // chaotic dyadic map
    double y = 0.1146125234634;

    // chaotic logistic map
    double r = 3.78;
    double z = 0.5;

    // chaotic duffing map
    double a = 2.75;
    double b = 0.15;
    double w = 0.5;
    double v = 0.0;

    // initialize default MEL logger
    init_logger();

    // initialize data logger to write immediately to file
    DataLogger data_logger(WriterType::Immediate, false);
    data_logger.set_header(
        {"Gauss", "Dyadic", "Logistic", "Duffing 0", "Duffing 1"});

    // data storage container
    std::vector<double> data_record(5);

    // initialize timing
    Timer timer(milliseconds(1), Timer::Hybrid);
    std::vector<double> delta_times;
    Time tick;
    Time tock;
    bool finished   = false;
    Time large_time = milliseconds(1);

    // open the data file for logging
    data_logger.open("my_immediate_datalog", ".", false);

    // begin logging data
    while (!stop && !finished) {
        // iterate chaotic mappings
        v = w;
        w = -b * v + a * w - std::pow(w, 3);
        x = std::exp(-alpha * x * x) + beta;
        if (y >= 0 && y < 0.5) {
            y = 2 * y;
        } else {
            y = 2 * y - 1.0;
        }
        z = r * z * (1 - z);

        // store data
        data_record[0] = v;
        data_record[1] = w;
        data_record[2] = x;
        data_record[3] = y;
        data_record[4] = z;

        // write to file and measure the time it takes
        tick = timer.get_elapsed_time_actual();
        data_logger.write(data_record);
        tock = timer.get_elapsed_time_actual();
        delta_times.push_back(
            (double)(tock.as_microseconds() - tick.as_microseconds()));

        // wait for duration of sample period
        timer.wait();

        // check for timeout
        if (timer.get_elapsed_time_actual() > immediate_write_duration) {
            finished = true;
        }
    }

    int count_large_times = 0;
    for (size_t i = 0; i < delta_times.size(); ++i) {
        if (delta_times[i] > large_time.as_microseconds())
            count_large_times++;
    }

    LOG(Info) << "Average write time for writing data immediately to file was "
              << mean(delta_times) << " us.";
    LOG(Info) << "Number of times immediate file writing took longer than "
              << large_time.as_microseconds() << " us was " << count_large_times
              << ".";

    // set data logger to write to buffer
    data_logger.set_writer_type(WriterType::Buffered);

    // reinitialize chaoitc map variables
    x = 0.1123565612351;
    y = 0.1146125234634;
    z = 0.5;
    w = 0.5;
    v = 0.0;

    // reset the timer
    timer.restart();
    finished = false;

    // begin logging data
    while (!stop && !finished) {
        // iterate chaotic mappings
        v = w;
        w = -b * v + a * w - std::pow(w, 3);
        x = std::exp(-alpha * x * x) + beta;
        if (y >= 0 && y < 0.5) {
            y = 2 * y;
        } else {
            y = 2 * y - 1.0;
        }
        z = r * z * (1 - z);

        // store data
        data_record[0] = v;
        data_record[1] = w;
        data_record[2] = x;
        data_record[3] = y;
        data_record[4] = z;

        // write to buffer
        data_logger.buffer(data_record);

        // wait for duration of sample period
        timer.wait();

        // check for timeout
        if (timer.get_elapsed_time_actual() > buffered_write_duration) {
            finished = true;
        }
    }

    tick = timer.get_elapsed_time_actual();
    data_logger.save_data("my_buffered_datalog", ".", false);
    data_logger.wait_for_save();
    data_logger.clear_data();
    tock = timer.get_elapsed_time_actual();
    LOG(Info) << "Time taken to save buffered data to file was "
              << tock.as_milliseconds() - tick.as_milliseconds() << " ms.";

    disable_realtime();
}

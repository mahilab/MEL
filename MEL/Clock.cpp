#include "Clock.h"
#include "util.h"
#include <boost/filesystem.hpp>

namespace mel {

    Clock::Clock(uint32 frequency, bool enable_logging) :
        frequency_(frequency),
        delta_time_(1.0 / frequency),
        tick_time_(std::chrono::nanoseconds(1000000000 / frequency)),
        enable_logging_(enable_logging),
        log_(DataLog("clock"))
    {
        if (enable_logging) {
            log_.add_col("Tick #").add_col("Elapsed (Ideal) [s]").add_col("Elapsed (Actual) [s]").add_col("Execution [s]").add_col("Wait [s]").add_col("Tick [s]");
        }
    }

    void Clock::start() {   
        stop_ = false;
        if (enable_logging_) {
            std::vector<double> row = { static_cast<double>(tick_count_) , elapsed_ideal_ , elapsed_actual_.count() * NS2S , elapsed_exe_.count() * NS2S , elapsed_wait_.count() * NS2S , elapsed_tick_.count() * NS2S };
            log_.add_row(row);
        }
    }

    void Clock::reset() {
        stop_ = true;
        tick_count_ = 0;
        start_ = std::chrono::high_resolution_clock::now();
        start_tick_ = start_;
        now_ = start_;
        elapsed_tick_ = std::chrono::nanoseconds(0);
        elapsed_actual_ = std::chrono::nanoseconds(0);
        elapsed_exe_ = std::chrono::nanoseconds(0);
        elapsed_wait_ = std::chrono::nanoseconds(0);
        elapsed_ideal_ = 0;
    }

    void Clock::restart() {
        reset();
        start();
    }

    void Clock::wait() {
        if (!stop_) {
            // increment sample number
            tick_count_ += 1;

            // update time variables
            now_ = std::chrono::high_resolution_clock::now();
            elapsed_tick_ = now_ - start_tick_;
            elapsed_actual_ = now_ - start_;
            elapsed_exe_ = elapsed_tick_;
            elapsed_ideal_ = tick_count_ * delta_time_;
            elapsed_wait_ = std::chrono::nanoseconds(0);

            // spinlock / busy wait until the next tick has been reached        
            while (elapsed_tick_ < tick_time_) {
                now_ = std::chrono::high_resolution_clock::now();
                elapsed_tick_ = now_ - start_tick_;
                elapsed_actual_ = now_ - start_;                
                elapsed_wait_ = elapsed_tick_ - elapsed_exe_;
            }

            // start the next tick
            start_tick_ = std::chrono::high_resolution_clock::now();

            // log this tick
            if (enable_logging_) {
                std::vector<double> row = { static_cast<double>(tick_count_) , elapsed_ideal_ , elapsed_actual_.count() * NS2S , elapsed_exe_.count() * NS2S , elapsed_wait_.count() * NS2S , elapsed_tick_.count() * NS2S };
                log_.add_row(row);
            }
        }
    }

    void Clock::stop() {
        stop_ = true;
        if (enable_logging_)
            log_.save_data("clock","clock_logs","true");
    }

}
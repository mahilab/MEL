#include "Clock.h"
#include "util.h"
#include <boost/filesystem.hpp>

namespace mel {

    Clock::Clock(uint32 frequency, bool enable_logging) :
        frequency_(frequency),
        tick_time_(std::chrono::nanoseconds(1000000000 / frequency)),
        enable_logging_(enable_logging)
    {
        if (enable_logging) {
            data_log_filename_ = log_dir_ + "\\clock_" + get_current_date_time() + ".txt";
            boost::filesystem::path dir(log_dir_.c_str());
            boost::filesystem::create_directory(dir);
            data_log_.open(data_log_filename_, std::ofstream::out | std::ofstream::trunc); // change trunc to app to append;
            data_log_ << "Tick #" << "\t" << "Elapsed (Ideal) [s]" << "\t" << "Elapsed (Actual) [s]" << "\t" 
                << "Execution [s]" << "\t" << "Wait [s]" << "\t" << "Tick [s]" << std::endl;
        }
    }

    uint32 Clock::get_tick() {
        return tick_count_;
    }

    double Clock::get_time() {
        return elapsed_ideal_.count() * NS2S;
    }

    void Clock::start() {
        
        tick_count_ = 0;
        start_ = std::chrono::high_resolution_clock::now();
        start_tick_ = start_;
        now_ = start_;
        elapsed_tick_ = std::chrono::nanoseconds(0);
        elapsed_actual_ = std::chrono::nanoseconds(0);
        elapsed_exe_ = std::chrono::nanoseconds(0);
        elapsed_wait_ = std::chrono::nanoseconds(0);
        elapsed_ideal_ = std::chrono::nanoseconds(0);
        offset_time_ = std::chrono::nanoseconds(0);
        if (enable_logging_) {
            data_log_ << tick_count_ << "\t" << elapsed_ideal_.count() * NS2S << "\t" << elapsed_actual_.count() * NS2S << "\t"
                << elapsed_exe_.count() * NS2S << "\t" << elapsed_wait_.count() * NS2S << "\t" << elapsed_tick_.count() * NS2S << std::endl;
        }
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

            // spinlock / busy wait until the next tick has been reached        
            while (elapsed_tick_ < tick_time_) {
                now_ = std::chrono::high_resolution_clock::now();
                elapsed_tick_ = now_ - start_tick_;
                elapsed_actual_ = now_ - start_;
                elapsed_ideal_ = tick_count_ * tick_time_;
                elapsed_wait_ = elapsed_tick_ - elapsed_exe_;
            }

            // start the next tick
            start_tick_ = std::chrono::high_resolution_clock::now();

            // log this tick
            if (enable_logging_) {
                data_log_ << tick_count_ << "\t" << elapsed_ideal_.count() * NS2S << "\t" << elapsed_actual_.count() * NS2S << "\t"
                    << elapsed_exe_.count() * NS2S << "\t" << elapsed_wait_.count() * NS2S << "\t" << elapsed_tick_.count() * NS2S << std::endl;
            }
        }
    }

    void Clock::stop() {
        stop_ = true;
    }

}
#pragma once
#include <chrono>
#include <fstream>
#include "util.h"


namespace mel {

    class Clock {
        
    public: 

        Clock(uint32 frequency, bool enabled_logging = false);

        uint32 tick();
        double time();        

        void start();
        void wait();
        void stop();
        void pause();
        void resume();

    private:

        const uint32 frequency_;     // the control loop sampling rate in Hz (e.g. 1000 Hz)
        uint32 tick_count_;          // the number or steps that have occured since that control loop was started 

        std::chrono::high_resolution_clock::time_point start_;      // time taken at the start of the control loop
        std::chrono::high_resolution_clock::time_point start_tick_; // time taken at the top of each new loop iteration
        std::chrono::high_resolution_clock::time_point now_;        // time taken a specific points

        double                   tick_time_d_;     // the control loop fixed step time or fundamental sample time in seconds (e.g. 0.001 seconds)
        std::chrono::nanoseconds tick_time_;       // the control loop fixed step time or fundamental sample time in nanoseconds (e.g. 0.001 seconds)
        std::chrono::nanoseconds elapsed_tick_;    // the amout of time that has elapsed during a single loop iteration                  
        std::chrono::nanoseconds elapsed_actual_;  // the actual amount of time that has elapsed since the control loop started
        std::chrono::nanoseconds elapsed_exe_;     // the actual amount of time that elapsed due to execution
        std::chrono::nanoseconds elapsed_wait_;    // the actual amount of tmee that elapsed due to waiting
        double                   elapsed_ideal_;   // the ideal ammount of time that has elapsed since the control loop started

        const double NS2S = 1.0 / 1000000000.0;

        bool stop_ = false;
        bool pause_ = false;

        // CLOCK DATA LOGGING

        bool enable_logging_;
        std::string   log_dir_ = "clock_logs";  // folder where data logs will be stored
        std::string   data_log_filename_;     // filename of the data log
        std::ofstream data_log_;              // stream for logging to the data log file

    };

}
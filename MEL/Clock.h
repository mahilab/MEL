#pragma once
#include <chrono>
#include "util.h"
#include "DataLog.h"

namespace mel {

    class Clock {
        
    public: 

        //---------------------------------------------------------------------
        // CONSTRUCTOR /  DESTRUCTOR
        //---------------------------------------------------------------------

        Clock(uint32 frequency);

        //---------------------------------------------------------------------
        // PUBLIC FUNCTIONS
        //---------------------------------------------------------------------

        /// Retruns the number of ticks or steps that have occured since that clock was started.
        /// This should only be called inside of a loop also calling the wait() function.
        uint32 tick() {
            return tick_count_;
        }
        /// Returns the ideal ammount of time that has elapsed since the clock was started or restarted.
        /// This should only be called inside of a loop also calling the wait() function.
        double time() {
            return elapsed_ideal_;
        }
        /// Returns the clock fixed step time or fundamental sample time in seconds.
        double delta_time() {
            return delta_time_;
        }

        /// Starts or restarts the clock timer and clears the log. This should be called before the wait loop starts.
        void start();
        /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
        /// execution time less than the fixed step time (delta time) of the clock.
        void wait();
        /// Logs this clocks time variables to a DataLog. Call this in the wait loop immediately after wait().
        void log();
        /// Saves the clock log to ./clock_logs/
        void save_log();

        /// Static function for simply blocking execution for a fixed amount of time.
        static void wait_for(double seconds);

    private:

        //---------------------------------------------------------------------
        // PRIVATE VARIABLES
        //---------------------------------------------------------------------

        const uint32 frequency_;  ///< the clock sampling rate in Hz (e.g. 1000 Hz)
        uint32 tick_count_;       ///< the number or steps that have occured since that clock was started 

        std::chrono::high_resolution_clock::time_point start_;      ///< time point when the clock was started
        std::chrono::high_resolution_clock::time_point start_tick_; ///< time point when the current tick wait was started
        std::chrono::high_resolution_clock::time_point now_;        ///< tme point taken at various times

        double                   delta_time_;      ///< the clock fixed step time or fundamental sample time in seconds (e.g. 0.001 seconds)
        std::chrono::nanoseconds tick_time_;       ///< the clock fixed step time or fundamental sample time in nanoseconds (e.g. 0.001 seconds)
        std::chrono::nanoseconds elapsed_tick_;    ///< the amout of time that has elapsed during a single wait loop iteration                  
        std::chrono::nanoseconds elapsed_actual_;  ///< the actual amount of time that has elapsed since the clock started
        std::chrono::nanoseconds elapsed_exe_;     ///< the actual amount of time that elapsed due to execution during the wait loop
        std::chrono::nanoseconds elapsed_wait_;    ///< the actual amount of time that elapsed due to waiting during the wait loop
        double                   elapsed_ideal_;   ///< the ideal ammount of time that has elapsed since the clock started

        const double NS2S = 1.0 / 1000000000.0;    ///< multiply by this to convert nanoseconds to seconds

        DataLog log_; ///< the clock DataLog

    };

}
#pragma once
#include <string>
#include <chrono>
#include "DataLog.h"
#include "mel_util.h"

namespace mel {

    namespace util {

        class Clock {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR /  DESTRUCTOR
            //---------------------------------------------------------------------

            Clock(uint32 frequency);

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            /// Starts or restarts the clock timer and clears the log. This should be called before the wait loop starts.
            void start();
            /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
            /// execution time less than the fixed step time (delta time) of the clock. This is a hybrid of accurate_wait()
            /// and efficient_wait(). It uses efficient_wait() for 75% of the remaining wait time, and accurate_wait() for
            /// the remaining 10%. It's performance cost is closest to efficient_wait(), and it's accuracy is nearly as good
            /// as accurate_wait(). 
            void wait();
            /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
            /// execution time less than the fixed step time (delta time) of the clock. This is a significantly more accurate
            /// version of efficient_wait(), but also uses significantly system resources and may cause other programs to lag.
            /// Use this if you aren't running real-time visualizations (e.g. Unity) on the same machine, or if your control
            /// algorithm depends on very accurate sampling rates (e.g. Kalman filter).
            void accurate_wait();
            /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
            /// execution time less than the fixed step time (delta time) of the clock. This is a significantly less accurate
            /// version of accurate_wait(), but uses fewer system resources. Use this if you are running real-time visualizations
            /// (e.g. Unity) on the same machine.
            void efficient_wait();
            /// Retruns the number of ticks or steps that have occured since that clock was started.
            /// This should only be called inside of a loop also calling the wait() function.
            uint32 tick();
            /// Returns the ideal ammount of time that has elapsed since the clock was started or restarted.
            /// This should only be called inside of a loop also calling the wait() function.
            double time();
            /// Logs this clocks time variables to a DataLog. Call this in the wait loop immediately after wait().
            void log();
            /// Saves the clock log to ./clock_logs/
            void save_log();

            //---------------------------------------------------------------------
            // PUBLIC VARIABLES
            //---------------------------------------------------------------------

            const uint32 frequency_;  ///< the clock sampling rate in Hz (e.g. 1000 Hz)
            const double delta_time_; ///< the clock fixed step time or fundamental sample time in seconds (e.g. 0.001 seconds)        

        private:

            //---------------------------------------------------------------------
            // PRIVATE VARIABLES
            //---------------------------------------------------------------------

            uint32 tick_count_;       ///< the number or steps that have occured since that clock was started 

            std::chrono::high_resolution_clock::time_point start_;      ///< time point when the clock was started
            std::chrono::high_resolution_clock::time_point start_tick_; ///< time point when the current tick wait was started
            std::chrono::high_resolution_clock::time_point now_;        ///< tme point taken at various times

            std::chrono::nanoseconds tick_time_;       ///< the clock fixed step time or fundamental sample time in nanoseconds (e.g. 0.001 seconds)
            std::chrono::nanoseconds elapsed_tick_;    ///< the amout of time that has elapsed during a single wait loop iteration                  
            std::chrono::nanoseconds elapsed_actual_;  ///< the actual amount of time that has elapsed since the clock started
            std::chrono::nanoseconds elapsed_exe_;     ///< the actual amount of time that elapsed due to execution during the wait loop
            std::chrono::nanoseconds elapsed_wait_;    ///< the actual amount of time that elapsed due to waiting during the wait loop
            double                   elapsed_ideal_;   ///< the ideal ammount of time that has elapsed since the clock started        

            util::DataLog log_; ///< the clock DataLog

            //---------------------------------------------------------------------
            // PRIVATE FUNCTIONS
            //---------------------------------------------------------------------

            void usleep(uint64 microseconds);

            //---------------------------------------------------------------------
            // STATIC VAIRABLES / FUNCTIONS
            //---------------------------------------------------------------------

        public:

            /// Static function for simply blocking execution for a fixed amount of time
            static void wait_for(double seconds);
            /// Returns the time since the application was started
            static double global_time();

        private:

            const static double NS2S; ///< multiply by this to convert nanoseconds to seconds
            const static std::chrono::high_resolution_clock::time_point global_start_; ///< time point when application was started
        };

    }
}
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
            /// execution time less than the fixed step time (delta time) of the clock. This is the most accurate wait
            /// function in MEL, but uses significantly system resources that efficient_wait() and hybrid_wait(). It may cause other 
            /// programs on the same machine to lag.
            void wait();
            /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
            /// execution time less than the fixed step time (delta time) of the clock. This is a significantly less accurate
            /// version of wait(), but uses fewer system resources. It is only capable of waiting for periods down to 0.5 - 0.75 ms. 
            /// It is HIGHLY reccommended to call util::enable_realtime() if using this waiting function, otherwise Window's thread 
            /// scheduler may not switch fast enough.
            void efficient_wait();
            /// Blocks execution until the current clock tick is over. This should be placed in a loop that has a normal
            /// execution time less than the fixed step time (delta time) of the clock. This is a hybrid of wait() and
            /// efficient_wait(). It uses efficient_wait() for 75% of the remaining wait time, and accurate_wait() for
            /// the remaining 25%. It's performance cost is closest to efficient_wait(), and it's accuracy is nearly as good
            /// as accurate_wait(). It is only capable of waiting for periods down to 0.5 - 0.75 ms. It is HIGHLY reccommended 
            /// to call util::enable_realtime() if using this waiting function, otherwise Window's thread scheduler may not 
            /// switch fast enough.
            void hybrid_wait();
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
            /// Benchmarks the clock on the current system and outputs results
            void benchmark();

            //---------------------------------------------------------------------
            // PUBLIC VARIABLES
            //---------------------------------------------------------------------

            const uint32 frequency_;  ///< the clock sampling rate in Hz (e.g. 1000 Hz)
            const double delta_time_; ///< the clock fixed step time or fundamental sample time in seconds (e.g. 0.001 seconds)        
            util::DataLog log_;       ///< the clock DataLog

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

            //---------------------------------------------------------------------
            // PRIVATE FUNCTIONS
            //---------------------------------------------------------------------

            /// Puts the thread to sleep so other processes can execute. A higher
            /// resolution version of std::this_thread::sleep_for() or Windows.h Sleep(). 
            void usleep(int64 microseconds);

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
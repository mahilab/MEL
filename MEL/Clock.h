#pragma once
#include <chrono>
#include "util.h"

namespace mel {

    class Clock {
        
    public: 

        Clock(uint frequency);

        uint get_step();
        double get_time();        

    private:

        friend class ControlLoop;

        void start();
        void wait();
        void pause();
        void resume();

        const uint frequency_;     // the control loop sampling rate in Hz (e.g. 1000 Hz)
        uint step_count_;          // the number or steps that have occured since that control loop was started 

        std::chrono::high_resolution_clock::time_point start_;      // time taken at the start of the control loop
        std::chrono::high_resolution_clock::time_point start_loop_; // time taken at the top of each new loop iteration
        std::chrono::high_resolution_clock::time_point now_;        // time taken a specific points

        std::chrono::nanoseconds step_time_;       // the control loop fixed step time or fundamental sample time in nanoseconds (e.g. 0.001 seconds)
        std::chrono::nanoseconds elapsed_loop_;    // the amout of time that has elapsed during a single loop iteration                  
        std::chrono::nanoseconds elapsed_actual_;  // the actual amount of time that has elapsed since the control loop started

        std::chrono::nanoseconds elapsed_ideal_;   // the ideal ammount of time that has elapsed since the control loop started

        const double NS2S = 1.0 / 1000000000.0;

    };

}
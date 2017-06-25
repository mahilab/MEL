#include "../include/Clock.h"

namespace mel {

    Clock::Clock(uint frequency) :
        frequency_(frequency),
        tick_time_(std::chrono::nanoseconds(1000000000 / frequency))
    {}

    uint Clock::get_tick() {
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
        elapsed_ideal_ = std::chrono::nanoseconds(0);
    }

    void Clock::wait() {
        
        // increment sample number
        tick_count_ += 1;

        // update time variables
        now_ = std::chrono::high_resolution_clock::now();
        elapsed_tick_ = now_ - start_tick_;
        elapsed_actual_ = now_ - start_;

        // spinlock / busy wait until the next tick has been reached
        while (elapsed_tick_ < tick_time_) {
            now_ = std::chrono::high_resolution_clock::now();
            elapsed_tick_ = now_ - start_tick_;
            elapsed_actual_ = now_ - start_;
            elapsed_ideal_ = tick_count_ * tick_time_;
        }

        // start the next tick
        start_tick_ = std::chrono::high_resolution_clock::now();      

    }

}
#include "Clock.h"

namespace mel {

    Clock::Clock(uint frequency) :
        frequency_(frequency),
        step_time_(std::chrono::nanoseconds(1000000000 / frequency))
    {

    }

    uint Clock::get_step() {
        return step_count_;
    }

    double Clock::get_time() {
        return elapsed_ideal_.count() * NS2S;
    }

    void Clock::start() {
        step_count_ = 0;
        start_ = std::chrono::high_resolution_clock::now();
        start_loop_ = start_;
        now_ = start_;
        elapsed_loop_ = std::chrono::nanoseconds(0);
        elapsed_actual_ = std::chrono::nanoseconds(0);
        elapsed_ideal_ = std::chrono::nanoseconds(0);
    }

    void Clock::wait() {
        
        // increment sample number
        step_count_ += 1;

        // update time variables
        now_ = std::chrono::high_resolution_clock::now();
        elapsed_loop_ = now_ - start_loop_;
        elapsed_actual_ = now_ - start_;

        // spinlock / busy wait the control loop until the loop rate has been reached
        while (elapsed_loop_ < step_time_) {
            now_ = std::chrono::high_resolution_clock::now();
            elapsed_loop_ = now_ - start_loop_;
            elapsed_actual_ = now_ - start_;
        }

        // update time variables
        start_loop_ = std::chrono::high_resolution_clock::now();
        elapsed_ideal_ = step_count_ * step_time_;

    }

}
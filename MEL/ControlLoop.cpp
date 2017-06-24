#include "ControlLoop.h"

bool ControlLoop::stop_ = false;

ControlLoop::ControlLoop(uint frequency) :
    frequency_(frequency),
    step_time_(std::chrono::nanoseconds(1000000000 / frequency))
{
    // register signal SIGINT and SIGBREAK with ctrl_c_handler */
    signal(SIGINT, ctrl_c_handler);
    signal(SIGBREAK, ctrl_c_handler);
}

void ControlLoop::execute(Controller* controller) {

    // call the Controller start function 
    controller->start();

    // update time variables
    step_count_ = 0;
    start_ = std::chrono::high_resolution_clock::now();
    elapsed_loop_ = std::chrono::nanoseconds(0);
    elapsed_actual_ = std::chrono::nanoseconds(0);

    // start the control loop
    while (!stop_) {

        // update time variables
        start_loop_ = std::chrono::high_resolution_clock::now();
        elapsed_ideal_ = step_count_ * step_time_;

        // call the Controller step function
        controller->step();

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
    }

    // call the Controller stop function
    controller->stop();

    // reset stop_
    stop_ = false;
}

void ControlLoop::ctrl_c_handler(int signum) {
    stop_ = true;
}

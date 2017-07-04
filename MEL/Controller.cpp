#include "Controller.h"

namespace mel {

    bool Controller::stop_ = false;
    bool Controller::pause_ = false;

    Controller::Controller(Clock& clock) :
        clock_(clock),
        stop_time_(-1)
    {
        // register signal SIGINT and SIGBREAK with ctrl_c_handler */
        signal(SIGINT, ctrl_c_handler);
        signal(SIGBREAK, ctrl_c_handler);
    }

    void Controller::queue_task(Task* controller) {
        // connect the controller Clock to the control loop Clock
        controller->clock_ = &clock_;
        // add the controller to the queue
        tasks_.push_back(controller);
    }

    void Controller::execute(uint32 stop_time_seconds) {

        stop_time_ = stop_time_seconds;

        // start the Controller(s)
        for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
            (*it)->start();

        // start the Clock
        clock_.start();

        // start the control loop
        while (!stop_ && clock_.time() <= stop_time_) {

            // step the Controller(s)
            for (auto it = tasks_.begin(); it != tasks_.end(); ++it)
                (*it)->step();

            // wait for the next clock tick
            clock_.wait();
        }

        // stop the Clock
        clock_.stop();

        // stop the Controller(s) in REVERSE order
        for (auto it = tasks_.rbegin(); it != tasks_.rend(); ++it)
            (*it)->stop();

        // reset stop_
        stop_ = false;
    }

    void Controller::ctrl_c_handler(int signum) {
        stop_ = true;
    }

    void Controller::ctrl_break_handler(int signum) {
        pause_ = !pause_;
    }

}
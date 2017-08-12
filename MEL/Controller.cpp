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

    void Controller::queue_task(Task* task) {
        // connect the task Clock to the controller Clock
        task->clock_ = &clock_;
        // add the controller to the task queues
        start_tasks_.push_back(task);
        step_tasks_.push_back(task);
        stop_tasks_.push_back(task);
    }

    void Controller::queue_start_task(Task* task) {
        task->clock_ = &clock_;
        start_tasks_.push_back(task);
    }

    void Controller::queue_step_task(Task* task) {
        task->clock_ = &clock_;
        step_tasks_.push_back(task);
    }

    void Controller::queue_stop_task(Task* task) {
        task->clock_ = &clock_;
        stop_tasks_.push_back(task);
    }

    void Controller::execute(uint32 stop_time_seconds) {

        stop_time_ = stop_time_seconds;

        // start the start Task(s)
        for (auto it = start_tasks_.begin(); it != start_tasks_.end(); ++it)
            (*it)->start();

        // start the Clock
        clock_.start();

        // start the control loop
        while (!stop_ && clock_.time() <= stop_time_) {

            // step the step Tasks(s)
            for (auto it = step_tasks_.begin(); it != step_tasks_.end(); ++it)
                (*it)->step();

            // wait for the next clock tick
            clock_.wait();
        }

        // stop the stop Tasks(s)
        for (auto it = stop_tasks_.begin(); it != stop_tasks_.end(); ++it)
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
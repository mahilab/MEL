#pragma once
#include <MEL/Legacy/Task.hpp>
#include <MEL/Utility/Clock.hpp>
#include <MEL/Utility/mel_util.hpp>

namespace mel {

class Controller {

public:

    // CONSTRUCTOR(S) / DESTRUCTOR(S)

    Controller(Clock& clock);

    // PUBLIC FUNCTIONS

    /// Adds a new Task to the #start_tasks_, #step_tasks, and #stop_tasks_ vectors of Tasks. All Task functions will be called  during Controller execution.
    void queue_task(Task* task);
    /// Adds a new Task to the list of start Tasks. Only the Task's start function will be called.
    void queue_start_task(Task* task);
    /// Adds a new Task to the list of step Tasks. Only the Task's step function will be called.
    void queue_step_task(Task* task);
    /// Adds a new Task to the list of stop Tasks. Only the Task's stop function will be called.
    void queue_stop_task(Task* task);
    /// Executes all Tasks queued in the  Controller in order or #start_tasks_, #step_tasks_, and #stop_tasks_ .
    void execute(uint32 stop_time_seconds = -1);

private:

    // TIME KEEPING

    Clock& clock_;     ///< the clock bound to the Controller
    uint32 stop_time_; ///< the time at which the Controller will end the stepping loop (-1 = inf)

    // SIGNAL HANDLING

    std::vector<Task*> start_tasks_; ///< vector of start Tasks
    std::vector<Task*> step_tasks_;  ///< vector of step Tasks
    std::vector<Task*> stop_tasks_;  ///< vector of stop Tasks

    static bool stop_;   ///< static boolean that will stop all Controllers (if multiple instances) when set to true
    static bool pause_;  ///< static boolean that will pause all Controllers (if multiple instances) when set to true

    static void ctrl_c_handler(int signum);      ///< signal handler that will set stop_ to true when CTRL+C is pressed at the terminal window
    static void ctrl_break_handler(int signum);  ///< signal handler that will set pause_ to true when CTRL+BREAK is pressed at the terminal window

    };

} // namespace mel

#pragma once
#include <chrono>
#include <csignal>
#include "Task.h"
#include "Clock.h"
#include "util.h"

namespace mel {

    class Controller {

    public:

        // CONSTRUCTOR / DESTRUCTOR

        Controller(Clock& clock);

        void queue_task(Task* controller);

        void execute(uint32 stop_time_seconds = -1);

        Clock& clock_;

    private:

        uint32 stop_time_ = -1;

        // SIGNAL HANDLING

        std::vector<Task*> tasks_;

        static bool stop_;   // static boolean that will stop all ControlLoops when set to true
        static bool pause_;  // static boolean that will pause all ControlLoops when set to true

        static void ctrl_c_handler(int signum);      // signal handler that will set stop_ to true when CTRL+C is pressed at the terminal window
        static void ctrl_break_handler(int signum);  // signal handler that will set pause_ to true when CTRL+BREAK is pressed at the terminal window

    };

}
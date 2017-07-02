#pragma once
#include <chrono>
#include <csignal>
#include "Controller.h"
#include "Clock.h"
#include "util.h"

namespace mel {

    class ControlLoop {

    public:

        // CONSTRUCTOR / DESTRUCTOR

        ControlLoop(Clock& clock, uint32 stop_time_seconds = -1);

        void queue_controller(Controller* controller);

        void execute();

        Clock& clock_;

    private:

        uint32 stop_time_ = -1;

        // SIGNAL HANDLING

        std::vector<Controller*> controllers_;

        static bool stop_;   // static boolean that will stop all ControlLoops when set to true
        static bool pause_;  // static boolean that will pause all ControlLoops when set to true

        static void ctrl_c_handler(int signum);      // signal handler that will set stop_ to true when CTRL+C is pressed at the terminal window
        static void ctrl_break_handler(int signum);  // signal handler that will set pause_ to true when CTRL+BREAK is pressed at the terminal window

    };

}
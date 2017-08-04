#pragma once
#include <chrono>
#include <csignal>
#include "Task.h"
#include "Clock.h"
#include "util.h"

namespace mel {


    class EventData {
    public:
        virtual ~EventData() {}
    };

    typedef EventData NoEventData;

    class StateMachine;

    class StateBase {
    public:
        virtual void invoke_state_action(StateMachine* sm, const EventData* data) const = 0;
    };

    template <class SM, class Data, void (SM::*Func)(const Data*)>
    class StateAction : public StateBase
    {
    public:

        virtual void invoke_state_action(StateMachine* sm, const EventData* data) const
        {
            // Downcast the state machine and event data to the correct derived type
            SM* derivedSM = static_cast<SM*>(sm);

            const Data* derivedData = dynamic_cast<const Data*>(data);

            // Call the state function
            (derivedSM->*Func)(derivedData);
        }
    };

    struct StateMapRow
    {
        const StateBase* const state;
    };

    class StateMachine {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        StateMachine(Clock& clock, int num_states, int initial_state = 0);


        // PUBLIC FUNCTIONS

        int get_current_state() { return current_state_; }

        int get_num_states() { return NUM_STATES; }

        void execute(uint32 stop_time_seconds = -1);

    protected:

        void event(int new_state, const EventData* data = nullptr);

        // TIME KEEPING

        Clock& clock_;     ///< the clock bound to the Controller
        uint32 stop_time_; ///< the time at which the Controller will end the stepping loop (-1 = inf)

        std::vector<Task*> start_tasks_; ///< vector of start Tasks
        std::vector<Task*> step_tasks_;  ///< vector of step Tasks
        std::vector<Task*> stop_tasks_;  ///< vector of stop Tasks

        static bool stop_;   ///< static boolean that will stop all Controllers (if multiple instances) when set to true
        static bool pause_;  ///< static boolean that will pause all Controllers (if multiple instances) when set to true

        // SIGNAL HANDLING

        static void ctrl_c_handler(int signum);      ///< signal handler that will set stop_ to true when CTRL+C is pressed at the terminal window
        static void ctrl_break_handler(int signum);  ///< signal handler that will set pause_ to true when CTRL+BREAK is pressed at the terminal window

    private:

        const int NUM_STATES;

        int current_state_;

        int new_state_;

        bool event_generated_;

        const EventData* event_data_;

        virtual const StateMapRow* get_state_map() = 0;

        void set_current_state(int new_state) { current_state_ = new_state; }


        

    };

}
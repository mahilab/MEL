#include "StateMachine.h"

namespace mel {

    bool StateMachine::ctrl_c_ = false;

    StateMachine::StateMachine(Clock& clock, int num_states, int initial_state) :
        clock_(clock),
        stop_time_(-1),
        NUM_STATES(num_states),
        current_state_(initial_state),
        new_state_(initial_state),
        event_generated_(false),
        event_data_(nullptr),
        stop_(false)
    {
        // register signal SIGINT and SIGBREAK with ctrl_c_handler */
        signal(SIGINT, ctrl_c_handler);
    }

    void StateMachine::event(int new_state, const EventData* data) {

        if (data == nullptr) {
            data = new NoEventData();
        }

        event_data_ = data;
        event_generated_ = true;
        new_state_ = new_state;
    }  


    void StateMachine::execute(uint32 stop_time_seconds) {

        const EventData* data_temp = nullptr;

        const StateMapRow* state_map = get_state_map();

        stop_time_ = stop_time_seconds;

        // start the Clock
        clock_.start();

        // run the state machine
        while (!ctrl_c_ && !stop_ && clock_.time() <= stop_time_) {


            if (event_generated_) {

                // Error check that the new state is valid before proceeding
                if (new_state_ >= NUM_STATES) {
                    std::cout << "Error: Invalid State" << std::endl;
                }

                // Switch to the new current state
                set_current_state(new_state_);

                // Copy of event data pointer
                data_temp = event_data_;

                // Event data used up, reset the pointer
                event_data_ = nullptr;

                // Event used up, reset the flag
                event_generated_ = false;

            }

            // Get the pointer from the state map
            const StateBase* state = state_map[current_state_].state;

            // Execute the state action passing in event data
            state->invoke_state_action(this, data_temp);

            // If event data was used, then delete it
            if (data_temp)
            {
                delete data_temp;
                data_temp = nullptr;
            }

            // wait for the next clock tick
            clock_.wait();
        }

        // stop the Clock (saves the DataLog)
        clock_.stop();

        if (ctrl_c_) {

            // run exit task
            ctrl_c_task();

            // reset ctrl_c_ and stop_
            ctrl_c_ = false;
            stop_ = false;
        }
    }

    void StateMachine::ctrl_c_handler(int signum) {
        ctrl_c_ = true;
    }


}
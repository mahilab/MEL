#include <MEL/Utility/StateMachine.hpp>
#include <MEL/Core/Console.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

StateMachine::StateMachine(int num_states, int initial_state) :
    NUM_STATES(num_states),
    current_state_(initial_state),
    new_state_(initial_state),
    event_generated_(false),
    event_data_(nullptr)
{

}

void StateMachine::event(int new_state, const EventData* data) {

    if (data == nullptr) {
        data = new NoEventData();
    }

    event_data_ = data;
    event_generated_ = true;
    new_state_ = new_state;
}


void StateMachine::execute() {

    const EventData* data_temp = nullptr;

    const StateMapRow* state_map = get_state_map();

    event_generated_ = true;

    // run the state machine
    while (event_generated_) {

        // Error check that the new state is valid before proceeding
        if (new_state_ >= NUM_STATES) {
            LOG(Error) << "Invalid state";
        }

        // Switch to the new current state
        set_current_state(new_state_);

        // Copy of event data pointer
        data_temp = event_data_;

        // Event data used up, reset the pointer
        event_data_ = nullptr;

        // Event used up, reset the flag
        event_generated_ = false;

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

    }


}

} // namespace mel

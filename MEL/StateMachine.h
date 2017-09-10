#pragma once
#include <string>

namespace mel {

    namespace util {

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

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            StateMachine(int num_states, int initial_state = 0);

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            int get_current_state() { return current_state_; }

            int get_num_states() { return NUM_STATES; }

            void execute();

        protected:

            void event(int new_state, const EventData* data = nullptr);

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

}
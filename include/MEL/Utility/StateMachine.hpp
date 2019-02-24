// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#pragma once


#include <string>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class EventData {
public:
    virtual ~EventData() {}
};

typedef EventData NoEventData;

class StateMachine;

class StateBase {
public:
    virtual void invoke_state_action(StateMachine* sm,
                                     const EventData* data) const = 0;
};

template <class SM, class Data, void (SM::*Func)(const Data*)>
class StateAction : public StateBase {
public:
    virtual void invoke_state_action(StateMachine* sm,
                                     const EventData* data) const {
        // Downcast the state machine and event data to the correct derived type
        SM* derivedSM = static_cast<SM*>(sm);

        const Data* derivedData = dynamic_cast<const Data*>(data);

        // Call the state function
        (derivedSM->*Func)(derivedData);
    }
};

struct StateMapRow {
    const StateBase* const state;
};

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class StateMachine {
public:
    StateMachine(int num_states, int initial_state = 0);

    int get_current_state() { return current_state_; }

    int get_num_states() { return NUM_STATES; }

    void execute();

protected:
    void event(int new_state, const EventData* data = nullptr);

private:
    virtual const StateMapRow* get_state_map() = 0;

    void set_current_state(int new_state) { current_state_ = new_state; }

private:
    const int NUM_STATES;

    int current_state_;

    int new_state_;

    bool event_generated_;

    const EventData* event_data_;
};

}  // namespace mel

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

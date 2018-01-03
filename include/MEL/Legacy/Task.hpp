#pragma once
// #include <MEL/Utility/Clock.hpp>

namespace mel {

class Task {

public:

    Task(std::string name) : name_(name) {}
    virtual ~Task() {};

    std::string name_; // the task name

protected:

    friend class Controller;  // give friendship to Controller so start(), step(), stop(), pause(), resume() and clock_ can be accessed


    // PURE VIRTUAL FUNCTIONS TO BE OVERRIDEN BY DERIVED CONTROLLERS

    virtual void start() {};  // override this function with code that should be called when the control loop starts
    virtual void step()  {};  // override this function with code that should be called with each iteration of the control loop
    virtual void stop()  {};  // override this function with code that should be called when the control loop stops

    // OPTIONAL VIRTUAL FUNCTIONS (NOT WORKING)

    virtual void pause() {};   // override this function with code that should be called if the control loop is paused
    virtual void resume() {};  // override this function with code that should be called if the control loop is resumed from a pause

    double time() {        // returns the ideal fixed rate time from the task clock
        //return clock_->time();
    }

    double delta_time() {
        //return clock_->delta_time_;
    }

private:

    //Clock* clock_;  // the Clock associated with this Task and the executing Controller (set automatically by Controller)

};

} // namespace mel

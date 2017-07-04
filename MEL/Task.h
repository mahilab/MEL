#pragma once
#include <string>
#include "Clock.h"

namespace mel {

    class Task {

    public:

        std::string name_ = "Rename Me"; // the controller name

    protected:

        friend class Controller;  // give friendship to ControlLoop so start(), step(), stop(), pause(), resume() and clock_ can be accessed       

        // PURE VIRTUAL FUNCTIONS TO BE OVERRIDEN BY DERIVED CONTROLLERS

        virtual void start() = 0;  // override this function with code that should be called when the control loop starts
        virtual void step()  = 0;  // override this function with code that should be called with each iteration of the control loop
        virtual void stop() =  0;  // override this function with code that should be called when the control loop stops

        // OPTIONAL VIRTUAL FUNCTIONS (NOT WORKING)

        virtual void pause() {};   // override this function with code that should be called if the control loop is paused
        virtual void resume() {};  // override this function with code that should be called if the control loop is resumed from a pause

        double time() {        // returns the ideal fixed rate time from the controller clock
            return clock_->time();
        }

    private:

        Clock* clock_;  // the Clock associated with this Controller and the executing ControlLoop (set automatically by ControlLoop)

    };

}
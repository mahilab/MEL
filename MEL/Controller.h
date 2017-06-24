#pragma once
#include <string>
#include <Windows.h>

namespace mel {

    class Controller {

    public:

        std::string name_ = "rename_me"; // the controller name
        

        // PURE VIRTUAL FUNCTIONS TO BE OVERRIDEN BY DERIVED CONTROLLERS

        virtual void start() = 0;  // override this function with code that should be called when the control loop starts
        virtual void step() = 0;  // override this function with code that should be called with each iteration of the control loop
        virtual void stop() = 0;  // override this function with code that should be called when the control loop stops

        // OPTIONAL VIRTUAL FUNCTIONS (NOT WORKING)

        virtual void pause() {};   // override this function with code that should be called if the control loop is paused
        virtual void resume() {};   // override this function with code that should be called if the control loop is resumed from a pause


    };

}
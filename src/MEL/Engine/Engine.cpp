#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Engine/Component.hpp>
#include <algorithm>

namespace mel {

Engine::Engine(Object* root_object) : 
    root_object_(root_object), 
    timer_(Frequency::Zero) 
{
    root_object_->set_engine(this);
}

Engine::~Engine() {
    
}

void Engine::run(Frequency loop_rate, Time duration) {

    // update Timer
    timer_ = Timer(loop_rate);
    
    // check if the engine contains Objects
    if (!root_object_) {
        LOG(Error) << "Cannot run the Engine because it contains no root Object";
        return;
    }

    LOG(Verbose) << "Starting Engine";

    // reset the stop flag
    stop_ = false;

    // start root Object
    root_object_->start_all();

    // restart the engine Timer
    timer_.restart();

    // run the Engine loop
    while (timer_.get_elapsed_time_actual() < duration && !stop_) {
        // update root Object 
        root_object_->update_all();
        // late update root Object
        root_object_->late_update_all();
        // wait Timer
        timer_.wait();
    }

    LOG(Verbose) << "Stopping Engine";

    // stop root Object
    root_object_->stop_all();
    
}

void Engine::stop() {
    stop_ = true;
}

}  // namespace mel

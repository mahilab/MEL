#include <MEL/Engine/Engine.hpp>
#include <MEL/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Engine/Component.hpp>
#include <algorithm>

namespace mel {

Engine::Engine(Object* root_object) : 
    timer_(Frequency::Zero) 
{
    set_root_object(root_object);
}

Engine::~Engine() {
    
}

void Engine::set_root_object(Object* root_object) {
    root_object_ = root_object;
    if (root_object_) {
        root_object_->set_engine(this);
    }
}

void Engine::run(Frequency loop_rate, Time duration) {
    run(Timer(loop_rate), duration);
}

void Engine::run(Timer timer, Time duration) {
    
    // check if the engine contains Objects
    if (!root_object_) {
        LOG(Fatal) << "Cannot run the Engine because it contains no root Object";
        return;
    }

    // enforce Component requirements
    if (!root_object_->enforce_requirements()) {
        LOG(Fatal) << "Cannot run the Engine because one or more Component requirements were not satisfied";
        return;
    }

    // update Timer
    timer_ = timer;

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

void Engine::reset() {
    if (root_object_) {
        root_object_->reset_all();
    }
}

}  // namespace mel

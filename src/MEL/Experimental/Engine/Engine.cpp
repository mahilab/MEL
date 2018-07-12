#include <MEL/Experimental/Engine/Engine.hpp>
#include <MEL/Experimental/Engine/Object.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Experimental/Engine/Component.hpp>
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

    // perform checks
    if (!perfom_checks())
        return;

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
    while (timer_.get_elapsed_time() < duration && !stop_) {
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

void Engine::run(uint32 iterations) {

    // perform checks
    if (!perfom_checks())
        return;

    LOG(Verbose) << "Starting Engine";

    // reset the stop flag
    stop_ = false;

    // start root Object
    root_object_->start_all();

    Clock clock;
    // run the Engine loop
    for (uint32 i = 0; i < iterations; ++i) {
        // update root Object
        root_object_->update_all();
        // late update root Object
        root_object_->late_update_all();
    }
    print(clock.get_elapsed_time());

    LOG(Verbose) << "Stopping Engine";

    // stop root Object
    root_object_->stop_all();
}

bool Engine::perfom_checks() {
    // check if the engine contains Objects
    if (!root_object_) {
        LOG(Fatal) << "Cannot run the Engine because it contains no root Object";
        return false;
    }
    return true;
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

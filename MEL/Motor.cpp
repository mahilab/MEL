#include "Motor.h"

namespace mel {

    Motor::Motor() :
        Actuator(),
        kt_(0.0),
        current_limit_(0.0),
        amp_gain_(0.0),
        has_current_sense_(false),
        ao_channel_(Daq::Ao()),
        do_channel_(Daq::Do()),
        ai_channel_(Daq::Ai()),
        current_(0.0),
        limited_current_(0.0), 
        current_sense_(0.0)
    { }

    Motor::Motor(std::string name, double kt, double current_limit, double amp_gain, Daq::Ao ao_channel) :
        Actuator(name, EnableMode::None),
        kt_(kt),
        current_limit_(current_limit),
        amp_gain_(amp_gain),
        has_current_sense_(false),
        ao_channel_(ao_channel),
        do_channel_(Daq::Do()),
        ai_channel_(Daq::Ai()),
        current_(0.0),
        limited_current_(0.0),
        current_sense_(0.0)
    { }

    Motor::Motor(std::string name, double kt, double current_limit, double amp_gain, Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode) :
        Actuator(name, enable_mode),
        kt_(kt),
        current_limit_(current_limit),
        amp_gain_(amp_gain),
        has_current_sense_(false),
        ao_channel_(ao_channel),
        do_channel_(do_channel),
        ai_channel_(Daq::Ai()),
        current_(0.0),
        limited_current_(0.0),
        current_sense_(0.0)
        
    { }

    Motor::Motor(std::string name, double kt, double current_limit, double amp_gain, Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel) :
        Actuator(name, enable_mode),
        kt_(kt),
        current_limit_(current_limit),
        amp_gain_(amp_gain),
        has_current_sense_(true),
        ao_channel_(ao_channel),
        do_channel_(do_channel),
        ai_channel_(ai_channel),
        current_(0.0),
        limited_current_(0.0),
        current_sense_(0.0)
    { }

    void Motor::enable() {
        enabled_ = true;
        if (enable_mode_ == EnableMode::High) {
            do_channel_.set_signal(1);
            do_channel_.daq_->write_digitals();
        }
        else if (enable_mode_ == EnableMode::Low) {
            do_channel_.set_signal(0);
            do_channel_.daq_->write_digitals();
        }
        else {
            std::cout << "WARNING: Motor <" << name_ << "> was not assigned an enable mode." << std::endl;
        }
    }

    void Motor::disable() {
        enabled_ = false;
        if (enable_mode_ == EnableMode::High) {
            ao_channel_.set_voltage(0);
            do_channel_.set_signal(0);
            do_channel_.daq_->write_all();
        }
        else if (enable_mode_ == EnableMode::Low) {
            ao_channel_.set_voltage(0);
            do_channel_.set_signal(1);
            do_channel_.daq_->write_all();
        }
        else {
            std::cout << "WARNING: Motor <" << name_ << "> was not assigned an enable mode." << std::endl;
        }
    }

    void Motor::set_torque(double new_torque) {
        torque_ = new_torque;
        set_current( torque_ / kt_ );
    }

    void Motor::set_current(double new_current) {
        current_ = new_current;
        limited_current_ = saturate_current(current_);
        ao_channel_.set_voltage(limited_current_ / amp_gain_);
    }

    double Motor::saturate_current(double new_current) {
        if (new_current > current_limit_) {
            return current_limit_;
        }
        else if (new_current < -current_limit_) {
            return -current_limit_;
        }
        else {
            return new_current;
        }
    }

    double Motor::get_current_sense() {
        if (has_current_sense_) {
            current_sense_ = ai_channel_.get_voltage();
            return current_sense_;
        }
        std::cout << "WARNING: Motor <" << name_ << "> was not constructed to enable current measurement. Returning 0." << std::endl;
        return 0.0;
    }    

}
#include "Motor.h"
#include "mel_util.h"
#include "mel_math.h"

namespace mel {

    namespace core {

        Motor::Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode,
            double hard_current_limit) :
            Actuator(name, enable_mode),
            kt_(kt),
            amp_gain_(amp_gain),
            ao_channel_(ao_channel),
            do_channel_(do_channel),
            continuous_current_limit_(hard_current_limit),
            peak_current_limit_(hard_current_limit),
            i2t_time_(0.0),
            current_limit_mode_(CurrentLimitMode::Saturate),
            has_current_limit_(true),
            has_current_sense_(false)
        { }


        Motor::Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode,
            double continuous_current_limit, double peak_current_limit, double i2t_time) :
            Actuator(name, enable_mode),
            kt_(kt),
            amp_gain_(amp_gain),
            ao_channel_(ao_channel),
            do_channel_(do_channel),
            continuous_current_limit_(continuous_current_limit),
            peak_current_limit_(peak_current_limit),
            i2t_time_(i2t_time),
            current_limit_mode_(CurrentLimitMode::I2T),
            has_current_limit_(true),
            has_current_sense_(false)
        { }

        Motor::Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel,
            double continuous_current_limit, double peak_current_limit, double i2t_time) :
            Actuator(name, enable_mode),
            kt_(kt),
            amp_gain_(amp_gain),
            ao_channel_(ao_channel),
            do_channel_(do_channel),
            ai_channel_(ai_channel),
            continuous_current_limit_(continuous_current_limit),
            peak_current_limit_(peak_current_limit),
            i2t_time_(i2t_time),
            current_limit_mode_(CurrentLimitMode::I2T),
            has_current_limit_(true),
            has_current_sense_(true)
        { }

        Motor::Motor(std::string name, double kt, double amp_gain,
            Daq::Ao ao_channel, Daq::Do do_channel, EnableMode enable_mode, Daq::Ai ai_channel,
            double hard_current_limit) :
            Actuator(name, enable_mode),
            kt_(kt),
            amp_gain_(amp_gain),
            ao_channel_(ao_channel),
            do_channel_(do_channel),
            ai_channel_(ai_channel),
            continuous_current_limit_(hard_current_limit),
            peak_current_limit_(hard_current_limit),
            i2t_time_(0.0),
            current_limit_mode_(CurrentLimitMode::Saturate),
            has_current_limit_(true),
            has_current_sense_(true)
        { }           

        void Motor::enable() {
            enabled_ = true;
            double i2t_integrand_ = 0;
            double i2t_integral_ = 0;
            double i2t_time_now_ = Clock::global_time();
            double i2t_time_last_ = Clock::global_time();
            if (enable_mode_ == EnableMode::High) {
                ao_channel_.set_voltage(0);
                do_channel_.set_signal(1);
                do_channel_.daq_->write_digitals();
            }
            else if (enable_mode_ == EnableMode::Low) {
                ao_channel_.set_voltage(0);
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
            set_current(torque_ / kt_);
        }

        void Motor::set_current(double new_current) {

            current_ = new_current;

            if (has_current_limit_) {
                if (current_limit_mode_ == CurrentLimitMode::Saturate)
                    limit_current_saturate();
                else if (current_limit_mode_ == CurrentLimitMode::I2T)
                    limit_current_i2t();
                ao_channel_.set_voltage(limited_current_ / amp_gain_);
            }
            else {
                ao_channel_.set_voltage(current_ / amp_gain_);
            }

        }

        double Motor::get_current_command() {
            return current_;
        }

        double Motor::get_current_limited() {
            return limited_current_;
        }

        double Motor::get_torque_command() {
            return current_ * kt_;
        }

        double Motor::get_torque_limited() {
            return limited_current_ * kt_;
        }

        void Motor::limit_current_saturate() {
            if (abs(current_) > continuous_current_limit_) {
                util::print("WARNING: Motor " + util::namify(name_) + " command current exceeded the hard current limit " + std::to_string(continuous_current_limit_) + " with a value of " + std::to_string(current_) + ".");
                limited_current_ = math::saturate(current_, continuous_current_limit_);
            }
        }

        void Motor::limit_current_i2t() {
            i2t_integrand_ = pow(limited_current_, 2) - pow(continuous_current_limit_, 2);
            i2t_time_now_ = Clock::global_time();
            i2t_integral_ = abs(i2t_integrand_ * (i2t_time_now_ - i2t_time_last_) + i2t_integral_);
            if (i2t_integral_ > i2t_time_ * (pow(peak_current_limit_, 2) - pow(continuous_current_limit_, 2))) {
                limited_current_ = math::saturate(current_, continuous_current_limit_);
                util::print("WARNING: Motor " + util::namify(name_) + " command current exceeded the I2T current limit: current saturated to " + std::to_string(continuous_current_limit_) + " with a commanded value of " + std::to_string(current_) + ".");
            }
            else {
                limited_current_ = math::saturate(current_, peak_current_limit_);
            }
            i2t_time_last_ = i2t_time_now_;
        }

        double Motor::get_current_sense() {
            if (has_current_sense_) {
                current_sense_ = ai_channel_.get_voltage();
                return current_sense_;
            }
            std::cout << "WARNING: Motor <" << name_ << "> was not constructed to enable current sense. Returning 0." << std::endl;
            return 0.0;
        }

    }

}
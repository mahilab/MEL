#include <MEL/Mechatronics/Amplifier.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

Amplifier::Amplifier() :
    Device("UNAMED AMMPLIFIER"),
    enable_level_(High),
    fault_level_(High),
    command_gain_(1.0),
    sense_gain_(1.0),
    current_command_(0.0),
    current_limiter_()
{
}

Amplifier::Amplifier(const std::string& name,
                     Logic enable_level,
                     DigitalOutput::Channel enable_channel,
                     double command_gain,
                     AnalogOutput::Channel command_channel,
                     Limiter current_limiter,
                     Logic fault_level,
                     DigitalInput::Channel fault_channel,
                     double sense_gain,
                     AnalogInput::Channel sense_channel)
    : Device(name),
      enable_level_(enable_level),
      fault_level_(fault_level),
      command_gain_(command_gain),
      sense_gain_(sense_gain),
      enable_channel_(enable_channel),
      fault_channel_(fault_channel),
      command_channel_(command_channel),
      sense_channel_(sense_channel),
      current_command_(0.0),
      current_limiter_(current_limiter)
{
}

void Amplifier::bind_enable_channel(DigitalOutput::Channel enable_channel) {
    enable_channel_ = enable_channel;
}

void Amplifier::bind_command_channel(AnalogOutput::Channel command_channel) {
    command_channel_ = command_channel;
}

void Amplifier::bind_fault_channel(DigitalInput::Channel fault_channel) {
    fault_channel_ = fault_channel;
}

void Amplifier::bind_sense_channel(AnalogInput::Channel sense_channel) {
    sense_channel_ = sense_channel;
}

void Amplifier::set_enable_level(Logic enable_level) {
    enable_level_ = enable_level;
}

void Amplifier::set_fault_level(Logic fault_level) {
    fault_level_ = fault_level;
}

void Amplifier::set_command_gain(double command_gain) {
    command_gain_ = command_gain;
}

void Amplifier::set_sense_gain(double sense_gain) {
    sense_gain_ = sense_gain;
}

void Amplifier::set_limiter(Limiter current_limiter) {
    current_limiter_ = current_limiter;
}

void Amplifier::set_current(double current) {
  current_command_ = current;
  command_channel_.set_value(current_limiter_.limit(current_command_) /
                             command_gain_);
}

double Amplifier::get_current_command() const {
  return current_command_;
}

double Amplifier::get_current_limited() const {
  return current_limiter_.get_limited_value();
}

double Amplifier::get_current_sense() const {
  if (sense_channel_.is_valid())
    return sense_gain_ * sense_channel_.get_value();
  LOG(Warning) << "WARNING: Amplifier " << get_name() << " current sense channel is invalid";
  return 0.0;
}

bool Amplifier::is_faulted(bool force_update) {
  if (force_update)
    fault_channel_.update();
  if (fault_channel_.get_value() == fault_level_)
      return true;
  else
      return false;
}

bool Amplifier::on_enable() {
    current_limiter_.reset();
    command_channel_.set_value(0.0);
    enable_channel_.set_value(enable_level_);
    if (enable_channel_.update())
        return true;
    else
        return false;
}

bool Amplifier::on_disable() {
    command_channel_.set_value(0.0);
    if (enable_level_ == High)
        enable_channel_.set_value(Low);
    else if (enable_level_ == Low)
        enable_channel_.set_value(High);

    if (enable_channel_.update())
        return true;
    else
        return false;
}

}  // namespace mel

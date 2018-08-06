#include <MEL/Daq/Encoder.hpp>
#include <MEL/Logging/Log.hpp>

namespace mel {

//==============================================================================
// MODULE DEFINITIONS
//==============================================================================

Encoder::Encoder() :
    has_velocity_(false),
    factors_(this),
    units_per_count_(this),
    positions_(this),
    conversions_(this),
    values_per_sec_(this),
    velocities_(this)
{
    compute_conversions();
}

Encoder::~Encoder() {

}

bool Encoder::reset_counts(const std::vector<int32>& counts) {
    values_.set(counts);
    return true;
}

bool Encoder::reset_count(uint32 channel_number, int32 count) {
    if (validate_channel_number(channel_number)) {
        values_[channel_number] = count;
        return true;
    }
    return false;
}


bool Encoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    factors_.set(factors);
    compute_conversions();
    return true;
}

bool Encoder::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
    if (validate_channel_number(channel_number)) {
        factors_[channel_number] = factor;
        compute_conversions();
        return true;
    }
    return false;
}

bool Encoder::zero() {
    std::vector<int32> zero(get_channel_count(), 0);
    return reset_counts(zero);
}

bool Encoder::zero_channel(uint32 channel_number) {
    int32 zero = 0;
    return reset_count(channel_number, zero);
}

void Encoder::set_units_per_count(const std::vector<double>& units_per_count) {
    units_per_count_.set(units_per_count);
    compute_conversions();
}

void Encoder::set_units_per_count(uint32 channel_number, double units_per_count) {
    if (validate_channel_number(channel_number)) {
        units_per_count_[channel_number] = units_per_count;
        compute_conversions();
    }
}

const std::vector<double>& Encoder::get_positions() {
    for (auto const& ch : get_channel_numbers())
        positions_[ch] = values_[ch] * conversions_[ch];
    return positions_.get();
}

double Encoder::get_position(uint32 channel_number) {
    if (validate_channel_number(channel_number)) {
        return values_[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

std::vector<double>& Encoder::get_values_per_sec() {
    if (!has_velocity_)
        LOG(Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    return values_per_sec_.get();
}

double Encoder::get_value_per_sec(uint32 channel_number) {
    if (!has_velocity_)
        LOG(Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    if (validate_channel_number(channel_number))
        return values_per_sec_[channel_number];
    else
        return double();
}

const std::vector<double>& Encoder::get_velocities() {
    if (!has_velocity_)
        LOG(Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    for (auto const& ch : get_channel_numbers())
        velocities_[ch] = values_per_sec_[ch] * conversions_[ch];
    return velocities_.get();
}

double Encoder::get_velocity(uint32 channel_number) {
    if (!has_velocity_)
        LOG(Warning) << "Encoder module " << get_name() << " has no velocity estimation";
    if (validate_channel_number(channel_number)) {
        return values_per_sec_[channel_number] * conversions_[channel_number];
    }
    else
        return double();
}

void Encoder::has_velocity(bool has_velocity) {
    has_velocity_ = has_velocity;
}

Encoder::Channel Encoder::get_channel(uint32 channel_number) {
    if (validate_channel_number(channel_number))
        return Channel(this, channel_number);
    else
        return Channel();
}

std::vector<Encoder::Channel> Encoder::get_channels(const std::vector<uint32>& channel_numbers) {
    std::vector<Channel> channels;
    for (std::size_t i = 0; i < channel_numbers.size(); ++i)
        channels.push_back(get_channel(channel_numbers[i]));
    return channels;
}

Encoder::Channel Encoder::operator[](uint32 channel_number) {
    return get_channel(channel_number);
}

std::vector<Encoder::Channel> Encoder::operator[](const std::vector<uint32>& channel_numbers) {
    return get_channels(channel_numbers);
}

void Encoder::compute_conversions() {
    for (std::size_t i = 0; i < get_channel_count(); ++i) {
        uint32 ch = get_channel_numbers()[i];
        conversions_[ch] = units_per_count_[ch] / static_cast<double>(factors_[ch]);
    }
}

//==============================================================================
// CHANNEL DEFINITIONS
//==============================================================================

Encoder::Channel::Channel() :
    ChannelBase(),
    PositionSensor("invalid_encoder"),
    VelocitySensor("invalid_encoder")
{ }

Encoder::Channel::Channel(Encoder* module, uint32 channel_number) :
    ChannelBase(module, channel_number),
    PositionSensor(module->get_name() + "[" + std::to_string(channel_number) + "]"),
    VelocitySensor(module->get_name() + "[" + std::to_string(channel_number) + "]")
{ }

bool Encoder::Channel::enable() {
    return Device::enable();
}

bool Encoder::Channel::disable() {
    return Device::disable();
}

double Encoder::Channel::get_position() {
    position_ = static_cast<Encoder*>(module_)->get_position(channel_number_);
    return position_;
}

double Encoder::Channel::get_value_per_sec() {
    return static_cast<Encoder*>(module_)->get_value_per_sec(channel_number_);
}

double Encoder::Channel::get_velocity() {
    velocity_ = static_cast<Encoder*>(module_)->get_velocity(channel_number_);
    return velocity_;
}

bool Encoder::Channel::zero() {
    return static_cast<Encoder*>(module_)->zero_channel(channel_number_);
}

bool Encoder::Channel::reset_count(int32 count) {
    return static_cast<Encoder*>(module_)->reset_count(channel_number_, count);
}

bool Encoder::Channel::set_quadrature_factor(QuadFactor factor) {
    return static_cast<Encoder*>(module_)->set_quadrature_factor(channel_number_, factor);
}

void Encoder::Channel::set_units_per_count(double units_per_count) {
    static_cast<Encoder*>(module_)->set_units_per_count(channel_number_, units_per_count);
}

} // namespace mel

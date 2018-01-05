#include <MEL/Daq/Encoder.hpp>

namespace mel {

//==============================================================================
// MODULE DEFINITIONS
//==============================================================================

Encoder::Encoder(const std::string& name, const std::vector<uint32>& channel_numbers) :
    Module<int32>(name, IoType::Input, channel_numbers),
    factors_(channel_count_, QuadFactor::X4),
    units_per_count_(channel_count_, 1.0),
    positions_(channel_count_, 0.0),
    conversions_(channel_count_,0.0)
{
    compute_conversions();
}

Encoder::~Encoder() {

}

bool Encoder::reset_counts(const std::vector<int32>& counts) {
    if (validate_channel_count(counts)) {
        values_ = counts;
        return true;
    }
    return false;
}

bool Encoder::reset_count(uint32 channel_number, int32 count) {
    if (validate_channel_number(channel_number)) {
        values_[channel_map_.at(channel_number)] = count;
        return true;
    }
    return false;
}


bool Encoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    if (validate_channel_count(factors)) {
        factors_ = factors;
        compute_conversions();
        return true;
    }
    return false;
}

bool Encoder::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
    if (validate_channel_number(channel_number)) {
        factors_[channel_map_.at(channel_number)] = factor;
        compute_conversions();
        return true;
    }
    return false;
}

bool Encoder::zero() {
    std::vector<int32> zero(channel_count_, 0);
    return reset_counts(zero);
}

bool Encoder::zero_channel(uint32 channel_number) {
    int32 zero = 0;
    return reset_count(channel_number, zero);
}

void Encoder::set_units_per_count(const std::vector<double>& units_per_count) {
    if (validate_channel_count(units_per_count)) {
        units_per_count_ = units_per_count;
        compute_conversions();
    }
}

void Encoder::set_units_per_count(uint32 channel_number, double units_per_count) {
    if (validate_channel_number(channel_number)) {
        units_per_count_[channel_map_.at(channel_number)] = units_per_count;
        compute_conversions();
    }
}

const std::vector<double>& Encoder::get_positions() {
    for (std::size_t i = 0; i < channel_count_; ++i)
        positions_[i] = values_[i] * conversions_[i];
    return positions_;
}

double Encoder::get_position(uint32 channel_number) {
    if (validate_channel_number(channel_number)) {
        std::size_t i = channel_map_.at(channel_number);
        return values_[i] * conversions_[i];
    }
    else
        return double();
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
    for (std::size_t i = 0; i < channel_count_; ++i)
        conversions_[i] = units_per_count_[i] / static_cast<double>(factors_[i]);
}

//==============================================================================
// CHANNEL DEFINITIONS
//==============================================================================

Encoder::Channel::Channel() :
    PositionSensor("invalid_encoder"),
    ChannelBase()
{ }    

Encoder::Channel::Channel(Encoder* module, uint32 channel_number) :
    PositionSensor(module->get_name() + "_encoder"),
    ChannelBase(module, channel_number)
{ }

bool Encoder::Channel::enable() {
    return Device::enable();
}

bool Encoder::Channel::disable() {
    return Device::disable();
}

double Encoder::Channel::get_position() {
    position_ = module_->get_position(channel_number_);
    return position_;
}

bool Encoder::Channel::zero() {
    return module_->zero_channel(channel_number_);
}

bool Encoder::Channel::reset_count(int32 count) {
    return module_->reset_count(channel_number_, count);
}

bool Encoder::Channel::set_quadrature_factor(QuadFactor factor) {
    return module_->set_quadrature_factor(channel_number_, factor);
}

void Encoder::Channel::set_units_per_count(double units_per_count) {
    module_->set_units_per_count(channel_number_, units_per_count);
}

} // namespace mel

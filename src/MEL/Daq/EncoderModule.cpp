#include <MEL/Daq/EncoderModule.hpp>

namespace mel {

//==============================================================================
// MODULE DEFINITIONS
//==============================================================================

EncoderModule::EncoderModule(const std::string& name, const std::vector<uint32>& channel_numbers) :
    InputModule(name, channel_numbers),
    factors_(channel_count_, QuadFactor::X4),
    units_per_count_(channel_count_, 1.0),
    positions_(channel_count_, 0.0),
    conversions_(channel_count_,0.0),
    invalid_channel_(EncoderModule::Channel(this, -1))
{
    EncoderModule::make_channels();
    compute_conversions();
}

EncoderModule::~EncoderModule() {

}

bool EncoderModule::reset_counts(const std::vector<int32>& counts) {
    if (validate_channel_count(counts)) {
        values_ = counts;
        return true;
    }
    return false;
}

bool EncoderModule::reset_count(uint32 channel_number, int32 count) {
    if (validate_channel_number(channel_number)) {
        values_[channel_map_.at(channel_number)] = count;
        return true;
    }
    return false;
}


bool EncoderModule::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    if (validate_channel_count(factors)) {
        factors_ = factors;
        compute_conversions();
        return true;
    }
    return false;
}

bool EncoderModule::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
    if (validate_channel_number(channel_number)) {
        factors_[channel_map_.at(channel_number)] = factor;
        compute_conversions();
        return true;
    }
    return false;
}

bool EncoderModule::zero() {
    std::vector<int32> zero(channel_count_, 0);
    return reset_counts(zero);
}

bool EncoderModule::zero_channel(uint32 channel_number) {
    int32 zero = 0;
    return reset_count(channel_number, zero);
}

void EncoderModule::set_units_per_count(const std::vector<double>& units_per_count) {
    if (validate_channel_count(units_per_count)) {
        units_per_count_ = units_per_count;
        compute_conversions();
    }
}

void EncoderModule::set_units_per_count(uint32 channel_number, double units_per_count) {
    if (validate_channel_number(channel_number)) {
        units_per_count_[channel_map_.at(channel_number)] = units_per_count;
        compute_conversions();
    }
}

const std::vector<double>& EncoderModule::get_positions() {
    for (std::size_t i = 0; i < channel_count_; ++i)
        positions_[i] = values_[i] * conversions_[i];
    return positions_;
}

double EncoderModule::get_position(uint32 channel_number) {
    if (validate_channel_number(channel_number)) {
        std::size_t i = channel_map_.at(channel_number);
        return values_[i] * conversions_[i];
    }
    else
        return double();
}

const EncoderModule::Channel& EncoderModule::get_channel(uint32 channel_number) {
    if (validate_channel_number(channel_number))
        return EncoderModule::channels_[channel_map_.at(channel_number)];
    else
        return EncoderModule::invalid_channel_;
}

void EncoderModule::make_channels() {
    for (std::size_t i = 0; i < channel_count_; ++i)
        EncoderModule::channels_.push_back(EncoderModule::Channel(this, channel_numbers_[i]));
}

void EncoderModule::compute_conversions() {
    for (std::size_t i = 0; i < channel_count_; ++i)
        conversions_[i] = units_per_count_[i] / static_cast<double>(factors_[i]);
}

//==============================================================================
// CHANNEL DEFINITIONS
//==============================================================================

EncoderModule::Channel::Channel() :
    Module<int32>::Channel(),
    PositionSensor("invalid_encoder"),
    module_(nullptr)
{ }    

EncoderModule::Channel::Channel(EncoderModule* module, uint32 channel_number) :
    Module<int32>::Channel(module, channel_number),
    PositionSensor(module->get_name() + "_encoder"),
    module_(module)
{ }

bool EncoderModule::Channel::enable() {
    return module_->enable();
}

bool EncoderModule::Channel::disable() {
    return module_->disable();
}

bool EncoderModule::Channel::zero() {
    return module_->zero_channel(channel_number_);
}

bool EncoderModule::Channel::offset(int32 offset_counts) {
    return module_->reset_count(channel_number_, offset_counts);
}

bool EncoderModule::Channel::set_quadrature_factor(QuadFactor factor) {
    return module_->set_quadrature_factor(channel_number_, factor);
}

void EncoderModule::Channel::set_units_per_count(double units_per_count) {
    module_->set_units_per_count(channel_number_, units_per_count);
}

double EncoderModule::Channel::get_position() {
    position_ = module_->get_position(channel_number_);
    return position_;
}

} // namespace mel

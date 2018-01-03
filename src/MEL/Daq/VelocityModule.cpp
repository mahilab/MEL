#include <MEL/Daq/VelocityModule.hpp>

namespace mel {

    //==============================================================================
    // MODULE DEFINITIONS
    //==============================================================================

    VelocityModule::VelocityModule(const std::string& name, const std::vector<uint32>& channel_numbers) :
        InputModule(name, channel_numbers),
        factors_(channel_count_, QuadFactor::X4),
        units_per_count_(channel_count_, 1.0),
        velocities_(channel_count_, 0.0),
        conversions_(channel_count_, 0.0),
        invalid_channel_(VelocityModule::Channel(this, -1))
    {
        VelocityModule::make_channels();
        compute_conversions();
    }

    VelocityModule::~VelocityModule() {

    }

    bool VelocityModule::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
        if (validate_channel_count(factors)) {
            factors_ = factors;
            compute_conversions();
            return true;
        }
        return false;
    }

    bool VelocityModule::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
        if (validate_channel_number(channel_number)) {
            factors_[channel_map_.at(channel_number)] = factor;
            compute_conversions();
            return true;
        }
        return false;
    }

    void VelocityModule::set_units_per_count(const std::vector<double>& units_per_count) {
        if (validate_channel_count(units_per_count)) {
            units_per_count_ = units_per_count;
            compute_conversions();
        }
    }

    void VelocityModule::set_units_per_count(uint32 channel_number, double units_per_count) {
        if (validate_channel_number(channel_number)) {
            units_per_count_[channel_map_.at(channel_number)] = units_per_count;
            compute_conversions();
        }
    }

    const std::vector<double>& VelocityModule::get_velocities() {
        for (std::size_t i = 0; i < channel_count_; ++i)
            velocities_[i] = values_[i] * conversions_[i];
        return velocities_;
    }

    double VelocityModule::get_velocity(uint32 channel_number) {
        if (validate_channel_number(channel_number)) {
            std::size_t i = channel_map_.at(channel_number);
            return values_[i] * conversions_[i];
        }
        else
            return double();
    }

    const VelocityModule::Channel& VelocityModule::get_channel(uint32 channel_number) {
        if (validate_channel_number(channel_number))
            return VelocityModule::channels_[channel_map_.at(channel_number)];
        else
            return VelocityModule::invalid_channel_;
    }

    void VelocityModule::make_channels() {
        for (std::size_t i = 0; i < channel_count_; ++i)
            VelocityModule::channels_.push_back(VelocityModule::Channel(this, channel_numbers_[i]));
    }

    void VelocityModule::compute_conversions() {
        for (std::size_t i = 0; i < channel_count_; ++i)
            conversions_[i] = units_per_count_[i] / static_cast<double>(factors_[i]);
    }

    //==============================================================================
    // CHANNEL DEFINITIONS
    //==============================================================================

    VelocityModule::Channel::Channel() :
        Module<double>::Channel(),
        VelocitySensor("invalid_velocity"),
        module_(nullptr)
    { }

    VelocityModule::Channel::Channel(VelocityModule* module, uint32 channel_number) :
        Module<double>::Channel(module, channel_number),
        VelocitySensor(module->get_name() + "_velocity"),
        module_(module)
    { }

    bool VelocityModule::Channel::enable() {
        return module_->enable();
    }

    bool VelocityModule::Channel::disable() {
        return module_->disable();
    }

    bool VelocityModule::Channel::set_quadrature_factor(QuadFactor factor) {
        return module_->set_quadrature_factor(channel_number_, factor);
    }

    void VelocityModule::Channel::set_units_per_count(double units_per_count) {
        module_->set_units_per_count(channel_number_, units_per_count);
    }

    double VelocityModule::Channel::get_velocity() {
        velocity_ = module_->get_velocity(channel_number_);
        return velocity_;
    }

} // namespace mel

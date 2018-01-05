#include <MEL/Daq/Velocity.hpp>

namespace mel {

    //==============================================================================
    // MODULE DEFINITIONS
    //==============================================================================

    Velocity::Velocity(const std::string& name, const std::vector<uint32>& channel_numbers) :
        Module<double>(name, IoType::Input, channel_numbers),
        factors_(channel_count_, QuadFactor::X4),
        units_per_count_(channel_count_, 1.0),
        velocities_(channel_count_, 0.0),
        conversions_(channel_count_, 0.0)
    {
        compute_conversions();
    }

    Velocity::~Velocity() {

    }

    bool Velocity::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
        if (validate_channel_count(factors)) {
            factors_ = factors;
            compute_conversions();
            return true;
        }
        return false;
    }

    bool Velocity::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
        if (validate_channel_number(channel_number)) {
            factors_[channel_map_.at(channel_number)] = factor;
            compute_conversions();
            return true;
        }
        return false;
    }

    void Velocity::set_units_per_count(const std::vector<double>& units_per_count) {
        if (validate_channel_count(units_per_count)) {
            units_per_count_ = units_per_count;
            compute_conversions();
        }
    }

    void Velocity::set_units_per_count(uint32 channel_number, double units_per_count) {
        if (validate_channel_number(channel_number)) {
            units_per_count_[channel_map_.at(channel_number)] = units_per_count;
            compute_conversions();
        }
    }

    const std::vector<double>& Velocity::get_velocities() {
        for (std::size_t i = 0; i < channel_count_; ++i)
            velocities_[i] = values_[i] * conversions_[i];
        return velocities_;
    }

    double Velocity::get_velocity(uint32 channel_number) {
        if (validate_channel_number(channel_number)) {
            std::size_t i = channel_map_.at(channel_number);
            return values_[i] * conversions_[i];
        }
        else
            return double();
    }

    Velocity::Channel Velocity::get_channel(uint32 channel_number) {
        if (validate_channel_number(channel_number))
            return Channel(this, channel_number);
        else
            return Channel();
    }

    std::vector<Velocity::Channel> Velocity::get_channels(const std::vector<uint32>& channel_numbers) {
        std::vector<Channel> channels;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i)
            channels.push_back(get_channel(channel_numbers[i]));
        return channels;
    }

    Velocity::Channel Velocity::operator[](uint32 channel_number) {
        return get_channel(channel_number);
    }

    std::vector<Velocity::Channel> Velocity::operator[](const std::vector<uint32>& channel_numbers) {
        return get_channels(channel_numbers);
    }

    void Velocity::compute_conversions() {
        for (std::size_t i = 0; i < channel_count_; ++i)
            conversions_[i] = units_per_count_[i] / static_cast<double>(factors_[i]);
    }

    //==============================================================================
    // CHANNEL DEFINITIONS
    //==============================================================================

    Velocity::Channel::Channel() :
        VelocitySensor("invalid_velocity"),
        ChannelBase()
    { }

    Velocity::Channel::Channel(Velocity* module, uint32 channel_number) :
        VelocitySensor(module->get_name() + "_velocity"),
        ChannelBase(module, channel_number)
    { }

    bool Velocity::Channel::enable() {
        return Device::enable();
    }

    bool Velocity::Channel::disable() {
        return Device::disable();
    }

    double Velocity::Channel::get_velocity() {
        velocity_ = module_->get_velocity(channel_number_);
        return velocity_;
    }

    bool Velocity::Channel::set_quadrature_factor(QuadFactor factor) {
        return module_->set_quadrature_factor(channel_number_, factor);
    }

    void Velocity::Channel::set_units_per_count(double units_per_count) {
        module_->set_units_per_count(channel_number_, units_per_count);
    }

} // namespace mel

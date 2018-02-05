#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QVelocity.hpp>
#include <MEL/Utility/System.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QVelocity::QVelocity(QDaq& daq, const std::vector<uint32>& channel_numbers) :
        Velocity(daq.name_ + "_velocity", channel_numbers),
        converted_channel_numbers_(convert_channel_numbers(channel_numbers)),
        daq_(daq)
    {
    }

    QVelocity::~QVelocity() {

    }

    bool QVelocity::enable() {
        return Device::enable();
    }

    bool QVelocity::disable() {
        return Device::disable();
    }

    bool QVelocity::update() {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_read_other(daq_.handle_, &converted_channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << name_ << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QVelocity::update_channel(uint32 channel_number) {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        uint32 converted_channel_number = convert_channel_number(channel_number);
        t_error result;
        result = hil_read_other(daq_.handle_, &converted_channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
        if (result == 0)
            return true;
        else {
            LOG(Error) << "Failed to update " << name_ << " channel number " << channel_number << " "
                << QDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QVelocity::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
        return Velocity::set_quadrature_factors(factors);
    }

    bool QVelocity::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
        return Velocity::set_quadrature_factor(channel_number, factor);
    }

    const std::vector<uint32>& QVelocity::get_converted_channel_numbers() {
        return converted_channel_numbers_;
    }


    uint32 QVelocity::convert_channel_number(uint32 channel_number) {
        if (channel_number < 14000)
            channel_number += 14000;
        return channel_number;
    }

    std::vector<uint32> QVelocity::convert_channel_numbers(const std::vector<uint32>& channel_numbers) {
        std::vector<uint32> new_channel_numbers = channel_numbers;
        for (std::size_t i = 0; i < channel_numbers.size(); ++i) {
            new_channel_numbers[i] = convert_channel_number(channel_numbers[i]);
        }
        return new_channel_numbers;
    }


} // namespace mel

#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Daq/Quanser/QuanserDI.hpp>
#include <MEL/Logging/Log.hpp>
#include <hil.h>

namespace mel {

    //==============================================================================
    // CLASS DEFINITIONS
    //==============================================================================

    QuanserDI::QuanserDI(QuanserDaq& daq, const std::vector<uint32>& channel_numbers) :
        Module(daq.get_name() + "_digital_input", IoType::InputOnly, channel_numbers),
        daq_(daq),
        quanser_values_(channel_count_, char(0))
    {
    }

    QuanserDI::~QuanserDI() {

    }

    bool QuanserDI::enable() {
        return Device::enable();
    }

    bool QuanserDI::disable() {
        return Device::disable();
    }

    bool QuanserDI::update() {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_read_digital(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &quanser_values_[0]);
        if (result == 0) {
            // convert Quanser t_boolean (aka char) to MEL Logic
            for (std::size_t i = 0; i < channel_count_; ++i)
                values_[i] = static_cast<Logic>(quanser_values_[i]);
            return true;
        }
        else {
            LOG(Error) << "Failed to update " << get_name() << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    bool QuanserDI::update_channel(uint32 channel_number) {
        if (!daq_.open_) {
            LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                       << daq_.get_name() << " is not open";
            return false;
        }
        t_error result;
        result = hil_read_digital(daq_.handle_, &channel_number, static_cast<uint32>(1), &quanser_values_[channel_map_.at(channel_number)]);
        if (result == 0) {
            values_[channel_map_.at(channel_number)] = static_cast<Logic>(quanser_values_[channel_map_.at(channel_number)]);
            return true;
        }
        else {
            LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "
                << QuanserDaq::get_quanser_error_message(result);
            return false;
        }
    }

    std::vector<char>& QuanserDI::get_quanser_values() {
        return quanser_values_;
    }




} // namespace mel

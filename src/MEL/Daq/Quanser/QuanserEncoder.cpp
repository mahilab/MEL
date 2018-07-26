#include <hil.h>
#include <MEL/Daq/Quanser/QuanserDaq.hpp>
#include <MEL/Daq/Quanser/QuanserEncoder.hpp>
#include <MEL/Logging/Log.hpp>
#include <MEL/Utility/System.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QuanserEncoder::QuanserEncoder(QuanserDaq& daq, const std::vector<uint32>& channel_numbers) :
    Encoder(daq.get_name() + "_encoder", channel_numbers),
    daq_(daq)
{
}

QuanserEncoder::~QuanserEncoder() {

}

bool QuanserEncoder::enable() {
    return Device::enable();
}

bool QuanserEncoder::disable() {
    return Device::disable();
}

bool QuanserEncoder::update() {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_read_encoder(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserEncoder::update_channel(uint32 channel_number) {
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_read_encoder(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
    if (result == 0)
        return true;
    else {
        LOG(Error) << "Failed to update " << get_name() << " channel number " << channel_number << " "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserEncoder::reset_counts(const std::vector<int32>& counts) {
    if (!Encoder::reset_counts(counts))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
                   << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_set_encoder_counts(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &counts[0]);
    sleep(milliseconds(10));
    if (result == 0) {
        LOG(Verbose) << "Reset " << get_name() << " counts to " << counts;
        return true;
    }
    else {
        LOG(Error) << "Failed to reset " << get_name() << " counts "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserEncoder::reset_count(uint32 channel_number, int32 count) {
    if (!Encoder::reset_count(channel_number, count))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    t_error result;
    result = hil_set_encoder_counts(daq_.handle_, &channel_number, static_cast<uint32>(1), &count);
    sleep(milliseconds(10));
    if (result == 0) {
        LOG(Verbose) << "Reset " << get_name() << " channel number " << channel_number << " count to " << count;
        return true;
    }
    else {
        LOG(Error) << "Failed to reset " << get_name() << " channel number " << channel_number << " count "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}


bool QuanserEncoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    if (!Encoder::set_quadrature_factors(factors))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    // convert MEL QuadFactor to Quanser t_encoder_quadratue_mode
    std::vector<t_encoder_quadrature_mode> converted_factors;
    for (auto it = factors_.begin(); it != factors_.end(); ++it) {
        if (*it == QuadFactor::X0)
            converted_factors.push_back(ENCODER_QUADRATURE_NONE);
        else if (*it == QuadFactor::X1)
            converted_factors.push_back(ENCODER_QUADRATURE_1X);
        else if (*it == QuadFactor::X2)
            converted_factors.push_back(ENCODER_QUADRATURE_2X);
        else if (*it == QuadFactor::X4)
            converted_factors.push_back(ENCODER_QUADRATURE_4X);
        else {
            LOG(Error) << "QuadFactor X" << static_cast<uint32>(*it) << " not supported by Quanser";
            return false;
        }
    }
    t_error result;
    result = hil_set_encoder_quadrature_mode(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &converted_factors[0]);
    sleep(milliseconds(10));
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " quadrature factors";
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " quadrature factors "
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

bool QuanserEncoder::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
    if (!Encoder::set_quadrature_factor(channel_number, factor))
        return false;
    if (!daq_.open_) {
        LOG(Error) << "Unable to call " << __FUNCTION__ << " because "
            << daq_.get_name() << " is not open";
        return false;
    }
    // convert MEL QuadFactor to Quanser t_encoder_quadratue_mode
    t_encoder_quadrature_mode converted_factor;
    if (factor == QuadFactor::X0)
        converted_factor = ENCODER_QUADRATURE_NONE;
    else if (factor == QuadFactor::X1)
        converted_factor = ENCODER_QUADRATURE_1X;
    else if (factor == QuadFactor::X2)
        converted_factor = ENCODER_QUADRATURE_2X;
    else if (factor == QuadFactor::X4)
        converted_factor = ENCODER_QUADRATURE_4X;
    else {
        LOG(Error) << "QuadFactor X" << static_cast<uint32>(factor) << " not supported by Quanser";
        return false;
    }
    t_error result;
    result = hil_set_encoder_quadrature_mode(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_factor);
    sleep(milliseconds(10));
    if (result == 0) {
        LOG(Verbose) << "Set " << get_name() << " channel number " << channel_number << " quadrature factor";
        return true;
    }
    else {
        LOG(Error) << "Failed to set " << get_name() << " channel number " << channel_number << " quadrature factor"
            << QuanserDaq::get_quanser_error_message(result);
        return false;
    }
}

} // namespace mel

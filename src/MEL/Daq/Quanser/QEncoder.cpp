#include <MEL/Daq/Quanser/QDaq.hpp>
#include <MEL/Daq/Quanser/QEncoder.hpp>
#include <MEL/Utility/System.hpp>
#include <hil.h>

namespace mel {

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

QEncoder::QEncoder(QDaq& daq, const std::vector<uint32>& channel_numbers) :
    EncoderModule(daq.name_ + "_encoder", channel_numbers),
    daq_(daq)
{
}

QEncoder::~QEncoder() {

}

bool QEncoder::enable() {
    print("Enabling " + namify(name_) + " ... Done");
    return Device::enable();
}

bool QEncoder::disable() {
    print("Disabling " + namify(name_) + " ... Done");
    return Device::disable();
}

bool QEncoder::update() {
    if (daq_.open_) {
        t_error result;
        result = hil_read_encoder(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &values_[0]);
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool QEncoder::update_channel(uint32 channel_number) {
    if (daq_.open_) {
        t_error result;
        result = hil_read_encoder(daq_.handle_, &channel_number, static_cast<uint32>(1), &values_[channel_map_.at(channel_number)]);
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool QEncoder::reset_counts(const std::vector<int32>& counts) {
    if (!EncoderModule::reset_counts(counts))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_set_encoder_counts(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &counts[0]);
        sleep(milliseconds(10));
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool QEncoder::reset_count(uint32 channel_number, int32 count) {
    if (!EncoderModule::reset_count(channel_number, count))
        return false;
    if (daq_.open_) {
        t_error result;
        result = hil_set_encoder_counts(daq_.handle_, &channel_number, static_cast<uint32>(1), &count);
        sleep(milliseconds(10));
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}


bool QEncoder::set_quadrature_factors(const std::vector<QuadFactor>& factors) {
    if (!EncoderModule::set_quadrature_factors(factors))
        return false;
    if (daq_.open_) {
        // convert MEL QuadFactor to Quanser t_encoder_quadratue_mode
        std::vector<t_encoder_quadrature_mode> converted_factors;
        for (auto it = factors_.begin(); it != factors_.end(); ++it) {
            if (*it == QuadFactor::X1)
                converted_factors.push_back(ENCODER_QUADRATURE_NONE);
            else if (*it == QuadFactor::X2)
                converted_factors.push_back(ENCODER_QUADRATURE_2X);
            else if (*it == QuadFactor::X4)
                converted_factors.push_back(ENCODER_QUADRATURE_4X);
            else {
                print("QuadFactor X" + stringify(static_cast<uint32>(*it)) + " not supported by Quanser");
                return false;
            }
        }
        t_error result;
        result = hil_set_encoder_quadrature_mode(daq_.handle_, &channel_numbers_[0], static_cast<uint32>(channel_count_), &converted_factors[0]);
        sleep(milliseconds(10));
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

bool QEncoder::set_quadrature_factor(uint32 channel_number, QuadFactor factor) {
    if (!EncoderModule::set_quadrature_factor(channel_number, factor))
        return false;
    if (daq_.open_) {
        // convert MEL QuadFactor to Quanser t_encoder_quadratue_mode
        t_encoder_quadrature_mode converted_factor;
        if (factor == QuadFactor::X1)
            converted_factor = ENCODER_QUADRATURE_NONE;
        else if (factor == QuadFactor::X2)
            converted_factor = ENCODER_QUADRATURE_2X;
        else if (factor == QuadFactor::X4)
            converted_factor = ENCODER_QUADRATURE_4X;
        else {
            print("QuadFactor X" + stringify(static_cast<uint32>(factor)) + " not supported by Quanser");
            return false;
        }
        t_error result;
        result = hil_set_encoder_quadrature_mode(daq_.handle_, &channel_number, static_cast<uint32>(1), &converted_factor);
        sleep(milliseconds(10));
        if (result == 0)
            return true;
        else {
            print(QDaq::get_quanser_error_message(result));
            return false;
        }
    }
    else {
        print(namify(daq_.get_name()) + " has not been opened; unable to call " + __FUNCTION__);
        return false;
    }
}

} // namespace mel
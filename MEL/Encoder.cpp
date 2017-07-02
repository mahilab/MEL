#include "Encoder.h"

namespace mel {

    Encoder::Encoder() :
        PositionSensor(),
        counts_per_unit_(0),
        count_(0.0),
        rate_(0.0)
    { }

    Encoder::Encoder(std::string name, uint32 counts_per_unit, Daq::EncoderChannel encoder_channel) :
        PositionSensor(name, false),
        counts_per_unit_(counts_per_unit),
        encoder_channel_(encoder_channel),
        count_(0.0),
        rate_(0.0)
    { }

    Encoder::Encoder(std::string name, uint32 counts_per_unit, Daq::EncoderChannel encoder_channel, Daq::EncRateChannel encrate_channel) :
        PositionSensor(name, true),
        counts_per_unit_(counts_per_unit),
        encoder_channel_(encoder_channel),
        encrate_channel_(encrate_channel),
        count_(0.0),
        rate_(0.0)
    { }

    double Encoder::get_position() {
        count_ = encoder_channel_.get_count();
        position_ = counts_per_unit_ * encoder_channel_.get_quadrature_factor() * count_;
        return position_;
    }

    double Encoder::get_velocity() {
        if (velocity_enabled_) {
            rate_ = encrate_channel_.get_rate();
            velocity_ = counts_per_unit_ * encoder_channel_.get_quadrature_factor() * rate_;
            return velocity_;
        }
        return PositionSensor::get_velocity(); // else call the default get_velocity() function
    }
}
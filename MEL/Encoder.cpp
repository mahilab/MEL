#include "Encoder.h"

namespace mel {

    Encoder::Encoder() :
        counts_per_unit_(0),
        count_(0.0),
        rate_(0.0),
        PositionSensor()
    {

    }

    Encoder::Encoder(uint32 counts_per_unit, Daq::EncoderChannel encoder_channel, Daq::EncRateChannel encrate_channel) :
        counts_per_unit_(counts_per_unit),
        encoder_channel_(encoder_channel),
        encrate_channel_(encrate_channel),
        count_(0.0),
        rate_(0.0),
        PositionSensor()
    {

    }

    double Encoder::get_position() {
        count_ = encoder_channel_.get_count();
        position_ = counts_per_unit_ * encoder_channel_.get_quadrature_factor() * count_;
        return position_;
    }

    double Encoder::get_velocity() {
        rate_ = encrate_channel_.get_rate();
        velocity_ = counts_per_unit_ * encoder_channel_.get_quadrature_factor() * rate_;
        return velocity_;
    }
}
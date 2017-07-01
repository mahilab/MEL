#include "Encoder.h"

namespace mel {

    Encoder::Encoder(double transmission, uint32 counts_per_rev, Daq::EncoderChannel encoder_channel, Daq::EncRateChannel encrate_channel) :
        transmission_(transmission),
        counts_per_rev_(counts_per_rev),
        encoder_channel_(encoder_channel),
        encrate_channel_(encrate_channel)
    {

    }

    double Encoder::get_position() {

        count_ = encoder_channel_.get_count();
        position_ = 2 * PI * transmission_ / (counts_per_rev_ * encoder_channel_.get_quadrature_factor()) * count_;

        return position_;
    }

    double Encoder::get_velocity() {

        rate_ = encrate_channel_.get_rate();
        velocity_ = 2 * PI * transmission_ / (counts_per_rev_ * encoder_channel_.get_quadrature_factor()) * rate_;

        return velocity_;
    }
}
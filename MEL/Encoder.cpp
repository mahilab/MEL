#include "Encoder.h"

namespace mel {

    Encoder::Encoder(double radius, uint32 counts_per_revolution, uint32 quadrature_factor, Daq* daq, uint32 daq_channel) :
        radius_(radius),
        counts_per_revolution_(counts_per_revolution),
        quadrature_factor_(quadrature_factor),
        daq_channel_(daq_channel),
        daq_(daq)
    {

    }

   // Encoder::Encoder(double radius, uint counts_per_revolution, uint quadrature_factor, Daq::EncoderChannel encoder_channel) :
   //     Encoder(radius, counts_per_revolution, quadrature_factor, encoder_channel.daq_, encoder_channel.channel_) {}


    double Encoder::get_count() {
        count_ = daq_->get_encoder_count(daq_channel_);
        return count_;
    }

    double Encoder::get_count_rate() {
        count_rate_ = daq_->get_encoder_rate(daq_channel_);
        return count_rate_;
    }
}
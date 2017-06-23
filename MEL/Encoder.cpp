#include "Encoder.h"

Encoder::Encoder(double transmission, int counts_per_revolution, int quadrature_factor, Daq* daq, int daq_channel) :
    transmission_(transmission),
    counts_per_revolution_(counts_per_revolution),
    quadrature_factor_(quadrature_factor),
    daq_channel_(daq_channel),
    daq_(daq) 
{

}


double Encoder::get_count() {
    count_ = daq_->get_encoder_count(daq_channel_);
    return count_;
}

double Encoder::get_count_rate() {
    count_rate_ =  daq_->get_encoder_count_rate(daq_channel_);
    return count_rate_;
}
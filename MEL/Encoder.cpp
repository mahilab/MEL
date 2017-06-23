#include "Encoder.h"


double Encoder::get_counts() {
    return daq_->get_encoder_counts(daq_channel_);
}

double Encoder::get_velocity() {
    return daq_->get_encoder_counts_per_sec(daq_channel_);
}
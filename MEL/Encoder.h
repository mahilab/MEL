#pragma once
#include "util.h"
#include "Daq.h"

namespace mel {

    class Encoder {

    public:

        // parameters
        const double radius_{};
        const uint counts_per_revolution_{};
        const uint quadrature_factor_{};
        const uint daq_channel_{};

        // relatives
        Daq* daq_;

        // constructor
        Encoder() {}
        Encoder(double radius, uint counts_per_revolution, uint quadrature_factor, Daq* daq, uint daq_channel);
        //Encoder(double radius, uint counts_per_revolution, uint quadrature_factor, Daq::EncoderChannel encoder_channel);

        // state variables
        double count_;
        double count_rate_;

        // getters and setters of state variables
        double get_count();
        double get_count_rate();

    };
}
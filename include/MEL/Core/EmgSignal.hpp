// MIT License
//
// MEL - MAHI Exoskeleton Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// Author(s): Craig McDonald (craig.g.mcdonald@gmail.com)

#ifndef MEL_EMGSIGNAL_HPP
#define MEL_EMGSIGNAL_HPP

#include <MEL/Daq/Input.hpp>
#include <MEL/Math/Filter.hpp>
//#include <MEL/Utility/RingBuffer.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class EmgSignal {

public:

    class TeagerKaiserEngergyOperator {

    public:

        /// Default constructor
        TeagerKaiserEngergyOperator() :
            s_(std::vector<double>(n_, 0.0)) {};

        double tkeo(const double x);

        void reset();

    private:

        std::vector<double> s_;
        static const int n_ = 2;

    };


    /// Constructor
    EmgSignal(AnalogInput::Channel channel, 
              Filter hp_filter = Filter({ 0.814254556886246, -3.257018227544984, 4.885527341317476, -3.257018227544984, 0.814254556886246 }, { 1.000000000000000, -3.589733887112175, 4.851275882519415, -2.924052656162457, 0.663010484385890 }), // 4th-order Butterworth High-Pass at 0.05 normalized cutoff frequency
              Filter lp_filter = Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 }), // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency
              Filter tkeo_lp_filter = Filter({ 0.058451424277128e-6, 0.233805697108513e-6, 0.350708545662770e-6, 0.233805697108513e-6, 0.058451424277128e-6 }, { 1.000000000000000, -3.917907865391990, 5.757076379118074, -3.760349507694534, 0.921181929191239 })); // 4th-order Butterworth Low-Pass at 0.01 normalized cutoff frequency

    /// run all signal processing operations for a single time step
    void update_signal();

    /// resets the filter and tkeo internal memory
    void reset_signal();

    /// return the EMG voltage on the associated channel when channel was last updated
    Voltage get_raw_voltage();

    double get_hp_signal();

    double get_rect_signal();

    double get_env_signal();

    double get_tkeo_signal();

    double get_tkeo_rect_signal();

    double get_tkeo_env_signal();



private:

    AnalogInput::Channel channel_;
    Filter hp_filter_;
    Filter lp_filter_;
    TeagerKaiserEngergyOperator tkeo_;
    Filter tkeo_lp_filter_;
    //RingBuffer<voltage> buffer_;

    Voltage raw_voltage_;
    double hp_signal_;
    double rect_signal_;
    double env_signal_;
    double tkeo_signal_;
    double tkeo_rect_signal_;
    double tkeo_env_signal_;


};

} // namespace mel

#endif // MEL_EMGSIGNAL_HPP

//==============================================================================
// CLASS DOCUMENTATION
//==============================================================================

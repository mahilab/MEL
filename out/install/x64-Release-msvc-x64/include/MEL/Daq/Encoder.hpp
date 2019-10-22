// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2019 Mechatronics and Haptic Interfaces Lab - Rice University
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
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once

#include <MEL/Mechatronics/PositionSensor.hpp>
#include <MEL/Mechatronics/VelocitySensor.hpp>
#include <MEL/Daq/ChannelBase.hpp>
#include <MEL/Daq/Module.hpp>

namespace mel {

/// Encapsulates an incremental optical encoder module with 32-bit precision
class Encoder : public Module<int> {
public:
    class Channel;

    /// Default Constructor (creates an invlaid empty Encoder)
    Encoder();

    /// Constructor with specified channel numbers
    Encoder(const ChanNums& channel_numbers);

    /// Default destructor
    virtual ~Encoder();

    /// Calls the DAQ's API to set a single encoder counter
    virtual bool reset_count(ChanNum channel_number, int count) = 0;

    /// This function should call the DAQ's API to set all encoder counters
    virtual bool reset_counts(const std::vector<int>& counts);

    /// Zeros a single encoder channel
    bool zero_channel(ChanNum channel_number);

    /// Zeros all encoder channels (calls reset_counts({0,...,0}))
    bool zero();

    /// Sets the units per count on a single encoder channel
    void set_units_per_count(ChanNum channel_number, double units_per_count);

    /// Sets the units per count of all encoder channels.
    void set_units_per_count(const std::vector<double>& units_per_count);

        /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_position(ChanNum channel_number);

    /// Performs conversion to positions using factors_ and counts_per_unit
    const std::vector<double>& get_positions();

    /// Returns a Encoder::Channel
    Channel get_channel(ChanNum channel_number);

    /// Returns multiple Encoder::Channels
    std::vector<Channel> get_channels(const ChanNums& channel_numbers);

    /// Returns a Encoder::Channel
    Channel operator[](ChanNum channel_number);

    /// Returns multiple Encoder::Channels
    std::vector<Channel> operator[](const ChanNums& channel_numbers);

protected:

    /// Override to call compute_conversions
    virtual bool on_enable() override;

    /// This function should call the DAQ's API to set the quadrature factor on all encoder channels
    virtual bool set_quadrature_factors(const std::vector<QuadFactor>& factors);

    /// This function should call the DAQ's API to set the quadrature factor on a single channel
    virtual bool set_quadrature_factor(ChanNum channel_number, QuadFactor factor);

    /// Precomputes position conversion sclars (i.e. units_per_count_ / factors_)
    void compute_conversions();

protected:

    Registry<QuadFactor> factors_;      ///< The encoder quadrature factors (default X4)
    Registry<double> units_per_count_;  ///< The number of counts per unit of travel of the Encoder
    Registry<double> positions_;        ///< The calculated positions of the Encoder channels
    Registry<double> conversions_;      ///< Conversion scalars used to convert to positions

public:
    /// Encapsulates and Encoder channel (can be used as a PositionSensor)
    class Channel : public ChannelBase<int>, public PositionSensor {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(Encoder* module, ChanNum channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<int>::operator=;

        /// Gets the encoder position
        double get_position() override;

        /// Zeros the encoder count
        bool zero();

        /// Sets the encoder count to a specific value
        bool reset_count(int count);

        /// Sets the encoder units/count
        void set_units_per_count(double units_per_count);

    private:

        /// Sets the encoder quadrature factor
        bool set_quadrature_factor(QuadFactor factor);

    };
};

}  // namespace mel

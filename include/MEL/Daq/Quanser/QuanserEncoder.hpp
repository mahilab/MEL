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
#include <MEL/Daq/Encoder.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QuanserDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser implementation of Encoder
class QuanserEncoder : public Encoder {
public:
    class Channel;

    QuanserEncoder(QuanserDaq& daq, const ChanNums& channel_numbers, bool has_velocity);

    bool update() override;

    bool update_channel(ChanNum channel_number) override;

    bool reset_counts(const std::vector<int32>& counts) override;

    bool reset_count(ChanNum channel_number, int32 count) override;

    bool set_quadrature_factors(const std::vector<QuadFactor>& factors) override;

    bool set_quadrature_factor(ChanNum channel_number, QuadFactor factor) override;

        /// Set whether the Encoder enables velocity estimation
    bool has_velocity() const;

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    std::vector<double>& get_values_per_sec();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_value_per_sec(ChanNum channel_number);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    const std::vector<double>& get_velocities();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_velocity(ChanNum channel_number);

    /// Returns the Quanser encoder velocity channels
    const ChanNums get_quanser_velocity_channels();

    /// Returns a QuanserEncoder::Channel
    Channel get_channel(ChanNum channel_number);

    /// Returns multiple QuanserEncoder::Channels
    std::vector<Channel> get_channels(const ChanNums& channel_numbers);

    /// Returns a QuanserEncoder::Channel
    Channel operator[](ChanNum channel_number);

    /// Returns multiple QuanserEncoder::Channels
    std::vector<Channel> operator[](const ChanNums& channel_numbers);

private:

    QuanserDaq& daq_;                            ///< Reference to parent QDaq
    Registry<ChanNum> velocity_channel_numbes_;  ///< Converted channel numbers
    Registry<double> values_per_sec_;            ///< Counts per second if Encoder has velocity
    Registry<double> velocities_;                ///< The calculated velocities of the Encoder channels
    bool has_velocity_;                          ///< True if Encoder module has velocity estimation

public:

    /// Encapsulates and QuanserEncoder channel (can be used as a PositionSensor or VelocitySensor)
    class Channel : public Encoder::Channel, public VelocitySensor {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(QuanserEncoder* module, ChanNum channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<int>::operator=;

        /// Gets the encoder counts per second if available
        double get_value_per_sec();

        /// Gets the encoder velocity if available
        double get_velocity() override;

    };

};

}  // namespace mel

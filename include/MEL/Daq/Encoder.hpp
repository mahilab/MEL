// MIT License
//
// MEL - Mechatronics Engine & Library
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
// Author(s): Evan Pezent (epezent@rice.edu)

#ifndef MEL_ENCODER_HPP
#define MEL_ENCODER_HPP

#include <MEL/Config.hpp>
#include <MEL/Core/PositionSensor.hpp>
#include <MEL/Core/VelocitySensor.hpp>
#include <MEL/Daq/ChannelBase.hpp>
#include <MEL/Daq/Module.hpp>

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Encapsulates an incremental optical encoder module
class MEL_API Encoder : public Module<int32> {
public:
    class Channel;

    /// Default constructor
    Encoder();

    /// Default destructor
    virtual ~Encoder();

    /// This function should call the DAQ's API to set all encoder counters
    virtual bool reset_counts(const std::vector<int32>& counts);

    /// This function should call the DAQ's API to set a single encoder counter
    virtual bool reset_count(uint32 channel_number, int32 count);

    /// This function should call the DAQ's API to set the quadrature factor on
    /// all encoder channels
    virtual bool set_quadrature_factors(const std::vector<QuadFactor>& factors);

    /// This function should call the DAQ's API to set the quadrature factor on
    /// a single channel
    virtual bool set_quadrature_factor(uint32 channel_number, QuadFactor factor);

    /// Zeros all encoder channels
    bool zero();

    /// Zeros a single encoder channel
    bool zero_channel(uint32 channel_number);

    /// Sets the units per count of all encoder channels.
    void set_units_per_count(const std::vector<double>& units_per_count);

    /// Sets the units per count on a single encoder channel
    void set_units_per_count(uint32 channel_number, double units_per_count);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    const std::vector<double>& get_positions();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_position(uint32 channel_number);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    std::vector<double>& get_values_per_sec();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_value_per_sec(uint32 channel_number);

    /// Performs conversion to positions using #factors_ and #counts_per_unit
    const std::vector<double>& get_velocities();

    /// Performs conversion to position using #factors_ and #counts_per_unit
    double get_velocity(uint32 channel_number);

    /// Returns a Encoder::Channel
    Channel get_channel(uint32 channel_number);

    /// Returns multiple Encoder::Channels
    std::vector<Channel> get_channels(
        const std::vector<uint32>& channel_numbers);

    /// Returns a Encoder::Channel
    Channel operator[](uint32 channel_number);

    /// Returns multiple Encoder::Channels
    std::vector<Channel> operator[](const std::vector<uint32>& channel_numbers);

    /// Set whether the Encoder enables velocity estimation
    void has_velocity(bool has_velocity);

private:
    /// Precomputes position conversion sclars (i.e. #units_per_count_ / #factors_)
    void compute_conversions();

protected:
    bool has_velocity_;                       ///< True if Encoder module has velocity estimation

    ValueContainer<QuadFactor> factors_;      ///< The encoder quadrature factors
    ValueContainer<double> units_per_count_;  ///< The number of counts per unit of travel of the Encoder
    ValueContainer<double> positions_;        ///< The calculated positions of the Encoder channels
    ValueContainer<double> conversions_;      ///< Conversion scalars used to convert to positions
    ValueContainer<double> values_per_sec_;   ///< Counts per second if Encoder has velocity
    ValueContainer<double> velocities_;       ///< The calculated velocities of the Encoder channels

public:
    /// Encapsulates and Encoder channel (can be used as a PositionSensor)
    class MEL_API Channel : public ChannelBase<int32>, public PositionSensor, public VelocitySensor {
    public:
        /// Default constructor. Creates invalid channel
        Channel();

        /// Creates a valid channel.
        Channel(Encoder* module, uint32 channel_number);

        /// Inherit assignment operator for setting
        using ChannelBase<int32>::operator=;

        /// Enables the encoder
        bool enable() override;

        /// Disables the encoder
        bool disable() override;

        /// Gets the encoder position
        double get_position() override;

        /// Gets the encoder counts per second if available
        double get_value_per_sec();

        /// Gets the encoder velocity if available
        double get_velocity() override;

        /// Zeros the encoder count
        bool zero();

        /// Sets the encoder count to a specific value
        bool reset_count(int32 count);

        /// Sets the encoder quadrature factor
        bool set_quadrature_factor(QuadFactor factor);

        /// Sets the encoder units/count
        void set_units_per_count(double units_per_count);
    };
};

}  // namespace mel

#endif  // MEL_ENCODER_HPP

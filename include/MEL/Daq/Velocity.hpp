#pragma once

#include <MEL/Daq/Module.hpp>
#include <MEL/Core/VelocitySensor.hpp>

namespace mel {

    //==========================================================================
    // CLASS DECLARATION
    //==========================================================================

    /// Encapsulates an incremental optical encoder module
    class Velocity : public Module<double> {

    public:

        class Channel;

        /// Default constructor
        Velocity(const std::string& name, const std::vector<uint32>& channel_numbers);

        /// Default destructor
        virtual ~Velocity();

        /// This function should call the DAQ's API to set the quadrature factor on
        /// all encoder channels
        virtual bool set_quadrature_factors(const std::vector<QuadFactor>& factors) = 0;

        /// This function should call the DAQ's API to set the quadrature factor on
        /// a single channel
        virtual bool set_quadrature_factor(uint32 channel_number, QuadFactor factor) = 0;

    public:

        /// Sets the units per count of all encoder channels.
        void set_units_per_count(const std::vector<double>& units_per_count);

        /// Sets the units per count on a single encoder channel
        void set_units_per_count(uint32 channel_number, double units_per_count);

        /// Performs conversion to positions using #factors_ and #counts_per_unit
        const std::vector<double>& get_velocities();

        /// Performs conversion to position using #factors_ and #counts_per_unit
        double get_velocity(uint32 channel_number);

        /// Returns a Velocity::Channel
        Channel get_channel(uint32 channel_number);

        /// Returns multiple Velocity::Channels
        std::vector<Channel> get_channels(const std::vector<uint32>& channel_numbers);

        /// Returns a Velocity::Channel
        Channel operator[](uint32 channel_number);

        /// Returns multiple Velocity::Channels
        std::vector<Channel> operator[](const std::vector<uint32>& channel_numbers);

    private:

        /// Precomputes position conversion sclars (i.e. #units_per_count_ / #factors_)
        void compute_conversions();

    protected:

        std::vector<QuadFactor> factors_;     ///< The encoder quadrature factors
        std::vector<double> units_per_count_; ///< The number of counts per unit of travel of the Encoder
        std::vector<double> velocities_;      ///< The calculated positions of the Encoder channels
        std::vector<double> conversions_;     ///< Conversion scalars used to conver to positions

    public:

        /// Encapsulates and EncoderModule channel, which is also a PositionSensor
        class Channel : public VelocitySensor {

        public:

            /// Default constructor. Creates invalid channel
            Channel();
            /// Creates a valid channel.
            Channel(Velocity* module, uint32 channel_number);
            /// Enables the velocity sensor
            bool enable() override;
            /// Disables the velocity sensor
            bool disable() override;
            /// Gets the current velocity
            double get_velocity() override;
            /// Synchronizes the velocity sensor with the real-world
            bool update();
            /// Returns the current value of the channel
            double get_value() const;
            /// Returns the current value of the channel
            double operator()();
            /// Gets the channel number
            uint32 get_channel_number() const;
            /// Sets the encoder quadrature factor
            bool set_quadrature_factor(QuadFactor factor);
            /// Sets the encoder units/count
            void set_units_per_count(double units_per_count);

        private:

            Velocity* module_;             ///< The Module this channel is on
            const uint32 channel_number_;  ///< The physical channel number
        };

    };

} // namespace mel

  //============================================================================
  // CLASS DOCUMENTATION
  //============================================================================

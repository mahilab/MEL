#pragma once

#include <MEL/Daq/InputModule.hpp>
#include <MEL/Core/VelocitySensor.hpp>

namespace mel {

    //==========================================================================
    // CLASS DECLARATION
    //==========================================================================

    /// Encapsulates an incremental optical encoder module
    class VelocityModule : public InputModule<double> {

    public:

        /// Encapsulates and EncoderModule channel, which is also a PositionSensor
        class Channel : public Module<double>::Channel, public VelocitySensor {
        public:
            Channel();
            Channel(VelocityModule* module, uint32 channel_number);
            bool enable() override;
            bool disable() override;
            bool set_quadrature_factor(QuadFactor factor);
            void set_units_per_count(double units_per_count);
            double get_velocity() override;
        private:
            VelocityModule* module_;
        };

    public:

        /// Default constructor
        VelocityModule(const std::string& name, const std::vector<uint32>& channel_numbers);

        /// Default destructor
        virtual ~VelocityModule();

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

        /// Overrides get_channel() so that it returns an VelocityModule::Channel
        const Channel& get_channel(uint32 channel_number) override;

    private:

        /// Overrides make_channels() so that channels_ is filled with Encoder::Channels
        void make_channels() override;

        /// Precomputes position conversion sclars (i.e. #units_per_count_ / #factors_)
        void compute_conversions();

    protected:

        std::vector<QuadFactor> factors_;     ///< The encoder quadrature factors
        std::vector<double> units_per_count_; ///< The number of counts per unit of travel of the Encoder
        std::vector<double> velocities_;      ///< The calculated positions of the Encoder channels
        std::vector<double> conversions_;     ///< Conversion scalars used to conver to positions

        Channel invalid_channel_;             ///< Represents an invalid channel
        std::vector<Channel> channels_;       ///< Hides Input::channels_

    };

} // namespace mel

  //============================================================================
  // CLASS DOCUMENTATION
  //============================================================================

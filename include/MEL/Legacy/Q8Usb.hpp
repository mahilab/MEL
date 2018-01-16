#pragma once
#include <array>
#include <MEL/Core/Daq.hpp>

typedef struct tag_card * t_card;

namespace mel {

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Implementation of Quanser Q8 USB
class Q8Usb : public Daq {

public:

    class Options {

    public:

        enum class UpdateRate { Default = 0, Normal_1kHz = 1, Fast_8kHz = 2 };
        enum class EncDir { Default = 0, Reversed = 1 };
        enum class EncFilter { Default = 0, Filtered = 1 };
        enum class EncDetection { Default = 0, High = 0, Low = 1 };
        enum class EncReload { Default = 0, OnPulse = 1 };

        Options() :
            ai_min_voltages_           { -10, -10, -10, -10, -10, -10, -10, -10 },
            ai_max_voltages_           { +10, +10, +10, +10, +10, +10, +10, +10 },
            ao_min_voltages_           { -10, -10, -10, -10, -10, -10, -10, -10 },
            ao_max_voltages_           { +10, +10, +10, +10, +10, +10, +10, +10 },
            ao_initial_voltages_       { 0,   0,   0,   0,   0,   0,   0,   0 },
            ao_final_voltages_         { 0,   0,   0,   0,   0,   0,   0,   0 },
            ao_expire_voltages_        { 0,   0,   0,   0,   0,   0,   0,   0 },
            do_initial_signals_        { 0,   0,   0,   0,   0,   0,   0,   0 },
            do_final_signals_          { 0,   0,   0,   0,   0,   0,   0,   0 },
            do_expire_signals_         { 0,   0,   0,   0,   0,   0,   0,   0 },
            encoder_quadrature_factors_{ 4,   4,   4,   4,   4,   4,   4,   4 }
        {}

        std::array<voltage, 8> ai_min_voltages_;
        std::array<voltage, 8> ai_max_voltages_;
        std::array<voltage, 8> ao_min_voltages_;
        std::array<voltage, 8> ao_max_voltages_;
        std::array<voltage, 8> ao_initial_voltages_;
        std::array<voltage, 8> ao_final_voltages_;
        std::array<voltage, 8> ao_expire_voltages_;
        std::array<dsignal, 8> do_initial_signals_;
        std::array<dsignal, 8> do_final_signals_;
        std::array<dsignal, 8> do_expire_signals_;
        std::array<uint32, 8>  encoder_quadrature_factors_;


        struct AoMode {

            enum Mode {
                Default = 0,
                VoltageMode = 0,
                CurrentMode2 = 1, CurrentMode1 = 2, CurrentMode0 = 3,
                ControlMode2 = 4, ControlMode1 = 5, ControlMode0 = 6
            };

        private:

            friend class Options;
            friend class Q8Usb;

            Mode mode_ = Mode::Default;
            double kff_ = 1;
            double a0_ = 1;
            double a1_ = 1;
            double a2_ = 1;
            double b0_ = 1;
            double b1_ = 1;
            double post_ = 1;

        public:

            AoMode() {}
            AoMode(Mode mode, double kff, double a0, double a1, double a2, double b0, double b1, double post) :
                mode_(mode), kff_(kff), a0_(a0), a1_(a1), a2_(a2), b0_(b0), b1_(b1), post_(post) {}

        };

        uint32 decimation_ = 1;
        UpdateRate update_rate_ = UpdateRate::Fast_8kHz;
        std::array<EncDir, 8> enc_dir_ = { EncDir::Default };
        std::array<EncFilter, 8> enc_filter_ = { EncFilter::Default };
        std::array<EncDetection, 8> enc_a_ = { EncDetection::Default };
        std::array<EncDetection, 8> enc_b_ = { EncDetection::Default };
        std::array<EncDetection, 8> enc_z_ = { EncDetection::Default };
        std::array<EncReload, 8> enc_reload_ = { EncReload::Default };
        std::array<double, 8> enc_velocity_ = { 0.0 };
        std::array<AoMode, 8> ao_modes_ = { AoMode() };

    private:

        friend class Q8Usb;

        std::string build();

    }; // http://www.quanser.com/Products/quarc/documentation/q8_usb.html


public:

    Q8Usb(uint32 id,
        channel_vec ai_channels,
        channel_vec ao_channels,
        channel_vec di_channels,
        channel_vec do_channels,
        channel_vec enc_channels,
        Options options = Options(),
        bool sanity_check_on_enable = false);

    ~Q8Usb() override;

public:

    Options options_;

    void enable() override;
    void disable() override;
    void reset() override;
    void factory_reset();

    void zero_encoders() override;
    void offset_encoders(std::vector<int32> offset_counts);

    void read_analogs() override;
    void read_digitals() override;
    void read_encoders() override;
    void read_encrates() override;
    void read_all() override;

    void write_digitals() override;
    void write_analogs() override;
    void write_all() override;

    void start_watchdog(double watchdog_timeout) override;
    void reload_watchdog() override;
    void stop_watchdog() override;
    bool is_watchdog_expired() override;

    // void benchmark(uint32 samples) override;

    double get_encoder_rate(channel channel_number) override {
        channel_number += 14000;
        return Daq::get_encoder_rate(channel_number);
    }

    virtual void set_ai_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) override;
    virtual void set_ao_voltage_ranges(voltage_vec min_voltages, voltage_vec max_voltages) override;

    virtual void set_ao_expire_voltages(voltage_vec expire_voltages) override;
    virtual void set_do_expire_signals(dsignal_vec expire_signals) override;

    virtual void set_encoder_quadrature_factors(std::vector<uint32> quadrature_factors) override;

private:

    uint32 id_;
    t_card q8_usb_;
    char options_str_[4096];   // Quarc board specific options
    bool sanity_check_on_enable_;

    bool sanity_check();
    static channel_vec get_q8_encrate_channels(channel_vec enc_channels);

public:

    static int get_q8_usb_count();
    static bool check_digital_loopback(uint32 daq_id, channel digital_channel);
    static std::string get_quarc_error_message(int result);
    static void print_quarc_error_message(int result);

};

} // namespace mel
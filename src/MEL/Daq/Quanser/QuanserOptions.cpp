#include <MEL/Daq/Quanser/QuanserOptions.hpp>

namespace mel {

//==============================================================================
// CLASS DEFINTIONS
//==============================================================================

QuanserOptions::QuanserOptions() :
    update_rate_(UpdateRate::Fast),
    decimation_(1)
{ }

void QuanserOptions::set_decimation(uint32 decimation) {
    decimation_ = decimation;
}

void QuanserOptions::set_update_rate(UpdateRate update_rate) {
    update_rate_ = update_rate;
}

void QuanserOptions::set_encoder_direction(uint32 channel_number, EncoderDirection direction) {
    if (direction == EncoderDirection::Reversed)
        options_ += "enc" + std::to_string(channel_number) + "_dir=1;";
}

void QuanserOptions::set_encoder_filter(uint32 channel_number, EncoderFilter filter) {
    if (filter == EncoderFilter::Filtered)
        options_ += "enc" + std::to_string(channel_number) + "_filter=1;";

}

void QuanserOptions::set_encoder_detection_a(uint32 channel_number, EncoderDetection detection) {
    if (detection == EncoderDetection::Low)
        options_ += "enc" + std::to_string(channel_number) + "_a=1;";

}

void QuanserOptions::set_encoder_detection_b(uint32 channel_number, EncoderDetection detection) {
    if (detection == EncoderDetection::Low)
        options_ += "enc" + std::to_string(channel_number) + "_b=1;";

}

void QuanserOptions::set_encoder_detection_z(uint32 channel_number, EncoderDetection detection) {
    if (detection == EncoderDetection::Low)
        options_ += "enc" + std::to_string(channel_number) + "_z=1;";

}

void QuanserOptions::set_encoder_reload(uint32 channel_number, EncoderReload reload) {
    if (reload == EncoderReload::OnPulse)
        options_ += "enc" + std::to_string(channel_number) + "_reload=1;";

}

void QuanserOptions::set_encoder_velocity(uint32 channel_number, double velocity) {
    if (velocity > 0.0) {
        std::string v = std::to_string(velocity);
        v.resize(7);
        options_ += "enc" + std::to_string(channel_number) + "_velocity=" + v + ";";
    }
}

void QuanserOptions::set_analog_output_mode(uint32 channel_number, AoMode mode, double kff,
    double a0, double a1, double a2, double b0, double b1, double post) {

    std::string ch = "ch" + std::to_string(channel_number) + "_";

    std::string mode_str = std::to_string(static_cast<int>(mode));
    std::string kff_str  = std::to_string(kff);     kff_str.resize(7);
    std::string a0_str   = std::to_string(a0);      a0_str.resize(7);
    std::string a1_str   = std::to_string(a1);      a1_str.resize(7);
    std::string a2_str   = std::to_string(a2);      a2_str.resize(7);
    std::string b0_str   = std::to_string(b0);      b0_str.resize(7);
    std::string b1_str   = std::to_string(b1);      b1_str.resize(7);
    std::string post_str = std::to_string(post);    post_str.resize(7);

    options_ += ch + "mode=" + mode_str + ";";
    options_ += ch + "kff="  + kff_str  + ";";
    options_ += ch + "a0="   + a0_str   + ";";
    options_ += ch + "a1="   + a1_str   + ";";
    options_ += ch + "a2="   + a2_str   + ";";
    options_ += ch + "b0="   + b0_str   + ";";
    options_ += ch + "b1="   + b1_str   + ";";
    options_ += ch + "post=" + post_str + ";";
}

void QuanserOptions::set_led_mode(LedMode mode) {
    if (mode == LedMode::Auto)
        options_ += "led=auto;";
    else if (mode == LedMode::User)
        options_ += "led=user;";
}

void QuanserOptions::set_special_option(std::string option) {
    if (option[option.length()-1] != ';')
        option += ";";
    options_ += option;
}

std::string QuanserOptions::get_string() {
    std::string base_options = "";
    if (update_rate_ == UpdateRate::Fast)
        base_options += "update_rate=fast;";
    else
        base_options += "update_rate=normal;";
    base_options += "decimation=" + std::to_string(decimation_) + ";";
    return base_options + options_;
}

} // namespace mel

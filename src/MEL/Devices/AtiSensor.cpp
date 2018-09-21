#include <MEL/Devices/AtiSensor.hpp>

namespace mel {

//==============================================================================
// HELPER FUNCTIONS
//==============================================================================

inline double compute_value(const std::array<double, 6>& array1, const std::array<double, 6>& array2) {
    return  array1[0] * array2[0] +
            array1[1] * array2[1] +
            array1[2] * array2[2] +
            array1[3] * array2[3] +
            array1[4] * array2[4] +
            array1[5] * array2[5];
}

//==============================================================================
// CLASS DEFINITIONS
//==============================================================================

AtiSensor::AtiSensor() {

}

AtiSensor::AtiSensor(std::vector<Input<Voltage>::Channel> channels, Calibration calibration) :
    channels_(channels),
    calibration_(calibration)
{

}

void AtiSensor::set_channels(std::vector<Input<Voltage>::Channel> channels) {
    channels_ = channels;
}

void AtiSensor::set_calibration(Calibration calibration_matrix) {
    calibration_ = calibration_matrix;
}

void AtiSensor::offset() {
    bias_[0] = channels_[0].get_value();
    bias_[1] = channels_[1].get_value();
    bias_[2] = channels_[2].get_value();
    bias_[3] = channels_[3].get_value();
    bias_[4] = channels_[4].get_value();
    bias_[5] = channels_[5].get_value();
}

double AtiSensor::get_force(Axis axis) {
    update_biased_voltages();
    switch (axis)
    {
    case AxisX:
        return compute_value(calibration_.Fx, bSTG_);
    case AxisY:
        return compute_value(calibration_.Fy, bSTG_);
    case AxisZ:
        return compute_value(calibration_.Fz, bSTG_);
    default:
        return 0.0;
    }
}

std::vector<double> AtiSensor::get_forces() {
    update_biased_voltages();
    forces_[0] = compute_value(calibration_.Fx, bSTG_);
    forces_[1] = compute_value(calibration_.Fy, bSTG_);
    forces_[2] = compute_value(calibration_.Fz, bSTG_);
    return forces_;
}

double AtiSensor::get_torque(Axis axis) {
    update_biased_voltages();
    switch (axis)
    {
    case AxisX:
        return compute_value(calibration_.Tx, bSTG_);
    case AxisY:
        return compute_value(calibration_.Ty, bSTG_);
    case AxisZ:
        return compute_value(calibration_.Tz, bSTG_);
    default:
        return 0.0;
    }
}

std::vector<double> AtiSensor::get_torques() {
    update_biased_voltages();
    torques_[0] = compute_value(calibration_.Tx, bSTG_);
    torques_[1] = compute_value(calibration_.Ty, bSTG_);
    torques_[2] = compute_value(calibration_.Tz, bSTG_);
    return torques_;
}

void AtiSensor::update_biased_voltages() {
    bSTG_[0] = channels_[0].get_value() - bias_[0];
    bSTG_[1] = channels_[1].get_value() - bias_[1];
    bSTG_[2] = channels_[2].get_value() - bias_[2];
    bSTG_[3] = channels_[3].get_value() - bias_[3];
    bSTG_[4] = channels_[4].get_value() - bias_[4];
    bSTG_[5] = channels_[5].get_value() - bias_[5];
}

} // namespace mel

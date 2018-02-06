#include <MEL/Devices/AtiMini45.hpp>
#include <algorithm>

namespace mel {

//AtiMini45::AtiMini45() :
//    ForceSensor(),
//    calib_mat_(array_2D<double, 6, 6>(0))
//{ }

AtiMini45::AtiMini45(std::string name, std::vector<Input<Voltage>::Channel> ai_channels, array_2D<double, 6, 6> calib_mat) :
    ForceSensor(name, ai_channels),
    calib_mat_(calib_mat)
{ }

std::vector<double> AtiMini45::get_forces() {

    std::vector<double> voltages = get_voltages();
    std::array<double, 6> voltages_arr;
    std::copy_n(voltages.begin(), 6, voltages_arr.begin());
    std::array<double, 6> forces_arr = calib_mat_.multiply(voltages_arr);
    std::vector<double> forces(forces_arr.begin(), forces_arr.end());

    return forces_ = forces;
}

bool AtiMini45::enable() {
    return Device::enable();
}

bool AtiMini45::disable() {
    return Device::disable();
}

} // namespace mel

#include <MEL/Mechatronics/TorqueSensor.hpp>

namespace mel {

    TorqueSensor::TorqueSensor() :
        torques_(3)
    { }

    std::vector<double> TorqueSensor::get_torques() {
        return torques_;
    }

} // namespace mel

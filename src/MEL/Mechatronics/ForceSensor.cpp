#include <MEL/Mechatronics/ForceSensor.hpp>

namespace mel {

ForceSensor::ForceSensor() :
    forces_(3)
{ }

std::vector<double> ForceSensor::get_forces() {
    return forces_;
}

} // namespace mel

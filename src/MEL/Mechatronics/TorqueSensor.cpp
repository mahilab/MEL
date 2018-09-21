#include <MEL/Mechatronics/TorqueSensor.hpp>

namespace mel {

TorqueSensor::TorqueSensor() :
    torques_(3, 0.0)
{   
}

TorqueSensor::~TorqueSensor() {

}


} // namespace mel

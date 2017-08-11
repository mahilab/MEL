#include "AtiMini45.h"

namespace mel {

    AtiMini45::AtiMini45() :
        ForceSensor(),
        calib_mat_({double_vec(6,0),double_vec(6,0),double_vec(6,0),double_vec(6,0),double_vec(6,0),double_vec(6,0)})
    { }

    AtiMini45::AtiMini45(std::string name, std::vector<Daq::Ai> ai_channels, double_mat calib_mat) :
        ForceSensor(name, ai_channels),
        calib_mat_(calib_mat)
    { }

    double_vec AtiMini45::get_forces() {
        
        forces_ = mat_vec_multiply(calib_mat_, voltages_);

        return forces_;
    }

}
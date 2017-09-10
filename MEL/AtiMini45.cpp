#include "AtiMini45.h"

namespace mel {

    namespace hdw {

        AtiMini45::AtiMini45() :
            ForceSensor(),
            calib_mat_(util::array_2D<double, 6, 6>(0))
        { }

        AtiMini45::AtiMini45(std::string name, std::vector<core::Daq::Ai> ai_channels, util::array_2D<double, 6, 6> calib_mat) :
            ForceSensor(name, ai_channels),
            calib_mat_(calib_mat)
        { }

        double_vec AtiMini45::get_forces() {

            double_vec voltages = get_voltages();
            std::array<double, 6> voltages_arr;
            std::copy_n(voltages.begin(), 6, voltages_arr.begin());
            std::array<double, 6> forces_arr = calib_mat_.multiply(voltages_arr);
            double_vec forces(forces_arr.begin(), forces_arr.end());

            return forces_ = forces;
        }

    }

}
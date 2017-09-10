#pragma once
#include "ForceSensor.h"

namespace mel {

    namespace hdw {

        class AtiMini45 : public core::ForceSensor {

        public:

            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            AtiMini45();
            AtiMini45(std::string name, std::vector<core::Daq::Ai> ai_channels, util::array_2D<double, 6, 6> calib_mat);

            //---------------------------------------------------------------------
            // PUBLIC FUNCTIONS
            //---------------------------------------------------------------------

            double_vec get_forces() override;

        private:

            //---------------------------------------------------------------------
            // PRIVATE VARIABLES
            //---------------------------------------------------------------------

            util::array_2D<double, 6, 6> calib_mat_;

        };

    }

}
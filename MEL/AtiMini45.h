#pragma once
#include "ForceSensor.h"

namespace mel {

    class AtiMini45 : public ForceSensor {

    public:

        // CONSTRUCTOR(S) / DESTRUCTOR(S)

        AtiMini45();
        AtiMini45(std::string name, std::vector<Daq::Ai> ai_channels, double_mat calib_mat);

        // PUBLIC FUNCTIONS
        double_vec get_forces() override;

    private:

        // PRIVATE VARIABLES

        double_mat calib_mat_;

    };

}
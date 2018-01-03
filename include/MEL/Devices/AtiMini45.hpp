#pragma once
#include <MEL/Core/ForceSensor.hpp>
#include <vector>

namespace mel {

class AtiMini45 : public ForceSensor {

public:

    //---------------------------------------------------------------------
    // CONSTRUCTOR(S) / DESTRUCTOR(S)
    //---------------------------------------------------------------------

    AtiMini45();
    AtiMini45(std::string name, std::vector<Daq::Ai> ai_channels, array_2D<double, 6, 6> calib_mat);

    //---------------------------------------------------------------------
    // PUBLIC FUNCTIONS
    //---------------------------------------------------------------------

    std::vector<double> get_forces() override;

private:

    //---------------------------------------------------------------------
    // PRIVATE VARIABLES
    //---------------------------------------------------------------------

    array_2D<double, 6, 6> calib_mat_;

};

} // namespace mel

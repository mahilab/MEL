
#ifndef MEL_MAHIEXOIIFRC_HPP
#define MEL_MAHIEXOIIFRC_HPP

#include <MEL/Exoskeletons/MahiExoII/MahiExoII.hpp>
#include <MEL/Devices/AtiMini45.hpp>

namespace mel {

class MahiExoIIFrc : public virtual MahiExoII {

public:


    /// Constructor
    MahiExoIIFrc(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());


    AtiMini45 wrist_force_sensor_;

private:

    std::vector<double> wrist_forces_;
};

} // namespace mel

#endif // MEL_MAHIEXOIIFRC_HPP
#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmgFrc.hpp>

namespace mel {

    MahiExoIIEmgFrc::MahiExoIIEmgFrc(MeiiConfiguration configuration, MeiiParameters parameters) :
        MahiExoII(configuration, parameters),
        MahiExoIIEmg(configuration, parameters),
        MahiExoIIFrc(configuration, parameters)
    {}

}
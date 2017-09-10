#include "MahiExoIIEmgFrc.h"

namespace mel {

    MahiExoIIEmgFrc::MahiExoIIEmgFrc(Config configuration, Params parameters) :
        MahiExoII(configuration, parameters),
        MahiExoIIEmg(configuration, parameters),
        MahiExoIIFrc(configuration, parameters)
    {}
}
#include "MahiExoIIEmgFrc.h"

namespace mel {

    namespace hdw {

        MahiExoIIEmgFrc::MahiExoIIEmgFrc(Config configuration, Params parameters) :
            MahiExoII(configuration, parameters),
            MahiExoIIEmg(configuration, parameters),
            MahiExoIIFrc(configuration, parameters)
        {}

    }
}
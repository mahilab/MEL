#include "MahiExoIIEmgFrc.h"

MahiExoIIEmgFrc::MahiExoIIEmgFrc(Config configuration, Params parameters) :
    MahiExoII(configuration, parameters),
    MahiExoIIEmg(configuration, parameters),
    MahiExoIIFrc(configuration, parameters)
{}
#pragma once
#include "MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp"
#include "MEL/Exoskeletons/MahiExoII/MahiExoIIFrc.hpp"

namespace mel {


    class MahiExoIIEmgFrc : public MahiExoIIEmg, public MahiExoIIFrc {

    public:


        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        MahiExoIIEmgFrc(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());
    };


}
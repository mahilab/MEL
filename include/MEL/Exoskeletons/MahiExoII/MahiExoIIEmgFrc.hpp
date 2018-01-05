#pragma once
#include "MahiExoIIEmg.h"
#include "MahiExoIIFrc.h"

namespace mel {

    namespace exo {

        class MahiExoIIEmgFrc : public MahiExoIIEmg, public MahiExoIIFrc {

        public:

            struct Config : public MahiExoIIEmg::Config, public MahiExoIIFrc::Config {

            };

            struct Params : public MahiExoIIEmg::Params, public MahiExoIIFrc::Params {

            };


            //---------------------------------------------------------------------
            // CONSTRUCTOR(S) / DESTRUCTOR(S)
            //---------------------------------------------------------------------

            MahiExoIIEmgFrc(Config configuration, Params parameters = Params());
        };

    }

}
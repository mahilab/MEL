
#ifndef MEL_MAHIEXOIIEMGFRC_HPP
#define MEL_MAHIEXOIIEMGFRC_HPP

#include <MEL/Exoskeletons/MahiExoII/MahiExoIIEmg.hpp>
#include <MEL/Exoskeletons/MahiExoII/MahiExoIIFrc.hpp>

namespace mel {


    class MahiExoIIEmgFrc : public MahiExoIIEmg, public MahiExoIIFrc {

    public:


        //---------------------------------------------------------------------
        // CONSTRUCTOR(S) / DESTRUCTOR(S)
        //---------------------------------------------------------------------

        MahiExoIIEmgFrc(MeiiConfiguration configuration, MeiiParameters parameters = MeiiParameters());
    };


} // namespace mel

#endif // MEL_MAHIEXOIIEMGFRC_HPP
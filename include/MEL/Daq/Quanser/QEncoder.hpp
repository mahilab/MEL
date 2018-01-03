#pragma once

#include <MEL/Daq/EncoderModule.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

/// Quanser implementation of Encoder
class QEncoder : public EncoderModule {

public:

    QEncoder(QDaq& daq,  const std::vector<uint32>& channel_numbers);

    ~QEncoder();

    bool enable() override;

    bool disable() override;

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool reset_counts(const std::vector<int32>& counts) override;

    bool reset_count(uint32 channel_number, int32 count) override;

    bool set_quadrature_factors(const std::vector<QuadFactor>& factors) override;

    bool set_quadrature_factor(uint32 channel_number, QuadFactor factor) override;

private:

    QDaq& daq_;  ///< Reference to parent QDaq

};

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================

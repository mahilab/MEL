#pragma once

#include <MEL/Daq/InputModule.hpp>
#include <MEL/Utility/NonCopyable.hpp>

namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class QAnalogInput : public InputModule<voltage>, NonCopyable {

public:

    QAnalogInput(QDaq& daq, const std::vector<uint32>& channel_numbers);

    ~QAnalogInput();

    bool enable() override;

    bool disable() override;

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool set_ranges(const std::vector<voltage>& min_values, const std::vector<voltage>& max_values) override;

    bool set_range(uint32 channel_number, voltage min_value, voltage max_value);

private:

    QDaq& daq_;  ///< Reference to parent QDaq

};

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================
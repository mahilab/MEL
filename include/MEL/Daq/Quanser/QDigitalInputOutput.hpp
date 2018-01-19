#pragma once

#include <MEL/Daq/InputOutput.hpp>
#include <MEL/Utility/NonCopyable.hpp>


namespace mel {

//==============================================================================
// FORWARD DECLARATIONS
//==============================================================================

class QDaq;

//==============================================================================
// CLASS DECLARATION
//==============================================================================

class QDigitalInputOutput : public InputOutput<logic>, NonCopyable {

public:

    QDigitalInputOutput(QDaq& daq, const std::vector<uint32>& channel_numbers, const std::vector<Direction>& directions);

    ~QDigitalInputOutput();

    bool enable() override;

    bool disable() override;

    bool update() override;

    bool update_channel(uint32 channel_number) override;

    bool set_directions(const std::vector<Direction>& directions) override;

    bool set_direction(uint32 channel_number, Direction direction) override;

    bool set_expire_values(const std::vector<logic>& expire_values) override;

    bool set_expire_value(uint32 channel_number, logic expire_value) override;

private:

    QDaq& daq_;  ///< Reference to parent QDaq

};

} // namespace mel

  //==============================================================================
  // CLASS DOCUMENTATION
  //==============================================================================
